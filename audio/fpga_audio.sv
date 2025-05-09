`define BGM_BEGIN 18'h0
`define BGM_END   18'h1387f     // 80 k samples ≈10 s @ 8 kHz

module fpga_audio (
    input  logic        clk,
    input  logic        reset,

    // handshake from Intel “Audio Core”
    input  logic        left_chan_ready,
    input  logic        right_chan_ready,

    // Avalon-MM slave (light-weight bridge, byte-addressed)
    input  logic [7:0]  writedata,
    input  logic        write,
    input  logic        chipselect,
    input  logic [3:0]  address,

    // sample stream to Audio Core
    output logic [15:0] sample_data_l,
    output logic        sample_valid_l,
    output logic [15:0] sample_data_r,
    output logic        sample_valid_r
);

    /* ----------------------------------------------------------------------
       Clip table:   index 0 = BGM, 1-5 = SFX
       All samples are 8-bit PCM @ 8 kHz, stored in audio_rom.hex
    ---------------------------------------------------------------------- */
    localparam int N_SFX = 6;
    logic [17:0] sound_begin_addresses [N_SFX-1:0] =
         '{18'h13880, 18'h17700, 18'h1b580, 18'h1dc52, 18'h1fb92, 18'h21ad2};
    logic [17:0] sound_end_addresses   [N_SFX-1:0] =
         '{18'h176ff, 18'h1b57f, 18'h1dc51, 18'h1fb91, 18'h21ad1, 18'h23fd3};

    /* ------------------------------------------------------------------ */
    logic [17:0] bgm_address, sound_address, sound_end_address;
    logic [7:0]  sound_data;
    logic        left_busy, right_busy, bgm_playing, sfx_playing;

    /* tiny synchronous ROM (in BRAM) */
    audio_rom rom_i (.addr(sound_address), .clk(clk), .q(sound_data));

    /* 8-bit → 16-bit (codec expects signed 16-bit samples) */
    assign sample_data_l = {sound_data, 8'h00};
    assign sample_data_r = sample_data_l;

    /* ------------------------------------------------------------------ */
    always_ff @(posedge clk) begin
        if (reset) begin
            sample_valid_l <= 0;
            sample_valid_r <= 0;
            left_busy      <= 0;
            right_busy     <= 0;
            bgm_address    <= `BGM_BEGIN;
            sound_address  <= `BGM_BEGIN;
            sound_end_address <= `BGM_END;
            bgm_playing    <= 0;
            sfx_playing    <= 0;

        end else if (chipselect && write) begin          // CPU write
            case (address)
                0: begin                                // start / stop BGM
                    bgm_playing <= writedata[0];
                    bgm_address <= `BGM_BEGIN;
                    if (!sfx_playing) begin
                        sound_address     <= `BGM_BEGIN;
                        sound_end_address <= `BGM_END;
                    end
                end
                1: begin                                // play SFX index
                    sound_address     <= sound_begin_addresses[writedata];
                    sound_end_address <= sound_end_addresses[writedata];
                    sfx_playing       <= 1;
                end
            endcase
        end

        /* ------------------------------------------------------------------
           feed one sample whenever both channels are ready
        ------------------------------------------------------------------ */
        if (bgm_playing || sfx_playing) begin
            if (left_chan_ready && right_chan_ready && !left_busy && !right_busy) begin
                /* reached end of current clip? */
                if (sound_address >= sound_end_address) begin
                    if (sfx_playing) begin              // SFX finished → resume BGM
                        sfx_playing       <= 0;
                        sound_address     <= bgm_address;
                        sound_end_address <= `BGM_END;
                    end else begin                      // loop BGM
                        bgm_address       <= `BGM_BEGIN;
                        sound_address     <= `BGM_BEGIN;
                        sound_end_address <= `BGM_END;
                    end
                end else begin                          // normal advance
                    sound_address <= sound_address + 1;
                    if (!sfx_playing) bgm_address <= bgm_address + 1;
                end
                /* issue sample */
                left_busy       <= 1;
                right_busy      <= 1;
                sample_valid_l  <= 1;
                sample_valid_r  <= 1;
            end else if (!left_chan_ready && !right_chan_ready) begin
                /* wait for Audio Core to de-assert ready */
                left_busy      <= 0;
                right_busy     <= 0;
                sample_valid_l <= 0;
                sample_valid_r <= 0;
            end
        end else begin
            sample_valid_l <= 0;
            sample_valid_r <= 0;
        end
    end
endmodule
