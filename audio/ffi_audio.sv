// -----------------------------------------------------------------------------
// ffi_audio.sv  – Forest Fire Ice custom audio IP (8 kHz / 8‑bit mono)
// -----------------------------------------------------------------------------
//  * Avalon‑MM slave (lightweight) – CONTROL register @ offset 0
//      bits [4:3]  clip_sel   (0‑3)   choose which clip to play once
//      bit  [0]    start_pulse        write 1 to trigger; auto‑clears
//  * Avalon‑ST source (dual‑channel) – to Intel audio_0 core
//      L_DATA / R_DATA : 16‑bit signed PCM (left & right send same sample)
//      VALID asserted for exactly 1 cycle at each 8 kHz sample edge
//
// Codec WM8731 requires ≥16‑bit I²S word – we expand each 8‑bit unsigned
// sample to 16‑bit signed by  left‑shifting 8 and centering (‑128).
//
// Clock domains: assume clk50 = 50 MHz system clock.
// -----------------------------------------------------------------------------

`timescale 1ns/1ps
module ffi_audio #(
    // Address bits for each clip ROM depth: 2^ADDR_WIDTH samples max
    parameter ADDR_WIDTH = 14,            // 16 384 samples ≈ 2.048 s @8 kHz
    // Names of memory‑initialisation files (8‑bit, unsigned, .mif)
    parameter CLIP0_FILE = "clip0.mif",
    parameter CLIP1_FILE = "clip1.mif",
    parameter CLIP2_FILE = "clip2.mif",
    parameter CLIP3_FILE = "clip3.mif"
)(
    // ---- global ----
    input  logic         clk50,
    input  logic         reset_n,

    // ---- Avalon‑MM slave (lightweight) ----
    input  logic         chipselect,      // connected to h2f_lw_axi_master
    input  logic  [3:0]  address,         // only 0 used
    input  logic  [31:0] writedata,
    input  logic         write,

    // ---- Avalon‑ST source to audio_0 ----
    output logic [15:0]  L_DATA,
    output logic [15:0]  R_DATA,
    output logic         L_VALID,
    output logic         R_VALID,
    input  logic         L_READY,
    input  logic         R_READY
);

// -----------------------------------------------------------------------------
// 1. CONTROL register : single word @ address 0
// -----------------------------------------------------------------------------
    typedef struct packed {
        logic [27:5] reserved;
        logic [1:0]  clip_sel;    // bits [4:3]
        logic        start;       // bit 0 : write 1 to start (auto‑clear)
    } ctrl_reg_t;

    ctrl_reg_t   ctrl_reg, next_ctrl_reg;

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            ctrl_reg <= '0;
        end else begin
            ctrl_reg <= next_ctrl_reg;
        end
    end

    always_comb begin
        next_ctrl_reg = ctrl_reg;
        if (chipselect && write && address==4'd0) begin
            next_ctrl_reg.clip_sel = writedata[4:3];
            next_ctrl_reg.start    = 1'b1;   // pulse set by CPU write
        end else begin
            // auto‑clear start bit after one clock so software needn't clear
            next_ctrl_reg.start    = 1'b0;
        end
    end

// -----------------------------------------------------------------------------
// 2. Sample‑rate generator : 50 MHz → 8 kHz (stereo frame) divider
//    50 000 000 / 8 000  = 6 250
// -----------------------------------------------------------------------------
    localparam DIVIDER = 6250 - 1;   // counts 0‑6249 inclusive
    logic [12:0] div_cnt;            // 13 bits enough for 6249
    logic        tick;               // asserted one cycle every sample

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            div_cnt <= DIVIDER;
            tick    <= 1'b0;
        end else begin
            if (div_cnt == 13'd0) begin
                div_cnt <= DIVIDER;
                tick    <= 1'b1;
            end else begin
                div_cnt <= div_cnt - 1'b1;
                tick    <= 1'b0;
            end
        end
    end

// -----------------------------------------------------------------------------
// 3. ROMs : four independent 8‑bit unsigned sample memories
// -----------------------------------------------------------------------------
    logic [ADDR_WIDTH-1:0] rom_addr;
    logic [7:0]            rom_dout0, rom_dout1, rom_dout2, rom_dout3;

    // generic ROM macro
    `define MAKE_ROM(INDEX, FILE) \
        (* ram_init_file = FILE, ramstyle = "M9K" *) \
        logic [7:0] rom``INDEX`` [0:(1<<ADDR_WIDTH)-1]; \
        initial $readmemh(FILE, rom``INDEX``);

    `MAKE_ROM(0, CLIP0_FILE)
    `MAKE_ROM(1, CLIP1_FILE)
    `MAKE_ROM(2, CLIP2_FILE)
    `MAKE_ROM(3, CLIP3_FILE)

    // read port – synchronous 1‑cycle latency
    always_ff @(posedge clk50) begin
        rom_dout0 <= rom0[rom_addr];
        rom_dout1 <= rom1[rom_addr];
        rom_dout2 <= rom2[rom_addr];
        rom_dout3 <= rom3[rom_addr];
    end

// -----------------------------------------------------------------------------
// 4. Playback state machine
// -----------------------------------------------------------------------------
    typedef enum logic [1:0] {IDLE, PLAY} state_t;
    state_t state, next_state;

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) state <= IDLE; else state <= next_state;
    end

    always_comb begin
        next_state = state;
        case (state)
            IDLE:  if (ctrl_reg.start) next_state = PLAY;
            PLAY:  if (rom_addr == {ADDR_WIDTH{1'b1}}) next_state = IDLE;
        endcase
    end

    // rom address counter
    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            rom_addr <= '0;
        end else if (state == IDLE && ctrl_reg.start) begin
            rom_addr <= '0;                 // restart at 0
        end else if (state == PLAY && tick && L_READY && R_READY) begin
            rom_addr <= rom_addr + 1'b1;    // advance each sample
        end
    end

// -----------------------------------------------------------------------------
// 5. Data path : 8‑bit unsigned → 16‑bit signed
// -----------------------------------------------------------------------------
    logic [7:0] rom_byte_mux;
    always_comb begin
        unique case (ctrl_reg.clip_sel)
            2'd0: rom_byte_mux = rom_dout0;
            2'd1: rom_byte_mux = rom_dout1;
            2'd2: rom_byte_mux = rom_dout2;
            2'd3: rom_byte_mux = rom_dout3;
            default: rom_byte_mux = 8'h00;
        endcase
    end

    // convert: center‑shift (‑128) and left‑shift 8 → signed 16‑bit
    logic signed [15:0] pcm16;
    assign pcm16 = { (rom_byte_mux - 8'd128), 8'b0 };

    // handshake to audio core
    logic valid_int;
    assign valid_int = (state == PLAY) && tick;   // one‑cycle pulse

    assign L_DATA  = pcm16;
    assign R_DATA  = pcm16;
    assign L_VALID = valid_int;
    assign R_VALID = valid_int;

endmodule
