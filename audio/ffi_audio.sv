// -----------------------------------------------------------------------------
// ffi_audio.sv  – Forest Fire Ice custom audio IP (8 kHz / **16-bit** mono)
// -----------------------------------------------------------------------------
//  * Avalon‑MM slave (lightweight) – CONTROL register @ offset 0
//      bits [4:3]  clip_sel   (0‑3)  choose clip once per write
//      bit  [0]    start_pulse        write 1 → start, auto‑clear
//  * Avalon‑ST source – to Intel audio_0 core (I²S 16‑bit)
//      L_DATA / R_DATA : 16‑bit signed PCM (mono duplicated)
//      VALID asserted 1 cycle at 8 kHz tick, gated by READY
//
// Clock: clk50 = 50 MHz. Sample divider 6250.
// -----------------------------------------------------------------------------

`timescale 1ns/1ps
module ffi_audio #(
    parameter ADDR_WIDTH = 13,               // 8192 samples ≈ 1.024 s @8 kHz
    parameter CLIP0_FILE = "clip0.mif",
    parameter CLIP1_FILE = "clip1.mif",
    parameter CLIP2_FILE = "clip2.mif",
    parameter CLIP3_FILE = "clip3.mif"
)(
    input  logic         clk50,
    input  logic         reset_n,

    /* Avalon‑MM slave */
    input  logic         chipselect,
    input  logic  [3:0]  address,
    input  logic  [31:0] writedata,
    input  logic         write,

    /* Avalon‑ST source */
    output logic [15:0]  L_DATA,
    output logic [15:0]  R_DATA,
    output logic         L_VALID,
    output logic         R_VALID,
    input  logic         L_READY,
    input  logic         R_READY
);

// -----------------------------------------------------------------------------
// 1. CONTROL register
// -----------------------------------------------------------------------------
    logic [1:0] clip_sel;
    logic       start_pulse;

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            clip_sel    <= 2'd0;
            start_pulse <= 1'b0;
        end else begin
            // CPU write
            if (chipselect && write && address==4'd0) begin
                clip_sel    <= writedata[4:3];
                start_pulse <= 1'b1;
            end else begin
                start_pulse <= 1'b0;           // auto‑clear
            end
        end
    end

// -----------------------------------------------------------------------------
// 2. 8 kHz tick generator – 50 MHz / 6250
// -----------------------------------------------------------------------------
    localparam DIVMAX = 13'd6249;
    logic [12:0] div_cnt;
    logic        tick;

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            div_cnt <= DIVMAX;
            tick    <= 1'b0;
        end else begin
            tick    <= (div_cnt==13'd0);
            div_cnt <= (tick ? DIVMAX : div_cnt-1'b1);
        end
    end

// -----------------------------------------------------------------------------
// 3. Clip ROMs – 16‑bit signed little‑endian
// -----------------------------------------------------------------------------
    logic [ADDR_WIDTH-1:0] rom_addr;

    (* ram_init_file = CLIP0_FILE, ramstyle = "M9K" *)
    logic signed [15:0] rom0 [0:(1<<ADDR_WIDTH)-1];
    (* ram_init_file = CLIP1_FILE, ramstyle = "M9K" *)
    logic signed [15:0] rom1 [0:(1<<ADDR_WIDTH)-1];
    (* ram_init_file = CLIP2_FILE, ramstyle = "M9K" *)
    logic signed [15:0] rom2 [0:(1<<ADDR_WIDTH)-1];
    (* ram_init_file = CLIP3_FILE, ramstyle = "M9K" *)
    logic signed [15:0] rom3 [0:(1<<ADDR_WIDTH)-1];

    logic signed [15:0] rom_dout;

    always_ff @(posedge clk50) begin
        case (clip_sel)
            2'd0: rom_dout <= rom0[rom_addr];
            2'd1: rom_dout <= rom1[rom_addr];
            2'd2: rom_dout <= rom2[rom_addr];
            default: rom_dout <= rom3[rom_addr];
        endcase
    end

// -----------------------------------------------------------------------------
// 4. Playback counter FSM
// -----------------------------------------------------------------------------
    typedef enum logic {IDLE, PLAY} state_t;
    state_t state;

    always_ff @(posedge clk50 or negedge reset_n) begin
        if (!reset_n) begin
            state    <= IDLE;
            rom_addr <= '0;
        end else begin
            case (state)
                IDLE: if (start_pulse) begin
                          state    <= PLAY;
                          rom_addr <= '0;
                       end
                PLAY: if (tick && L_READY && R_READY) begin
                          rom_addr <= rom_addr + 1'b1;
                          if (rom_addr == {ADDR_WIDTH{1'b1}})
                              state <= IDLE;   // reached end
                       end
            endcase
        end
    end

// -----------------------------------------------------------------------------
// 5. Avalon‑ST outputs
// -----------------------------------------------------------------------------
    logic valid_int;
    assign valid_int = (state==PLAY) && tick;

    assign L_DATA  = rom_dout;
    assign R_DATA  = rom_dout;
    assign L_VALID = valid_int;
    assign R_VALID = valid_int;

endmodule
