`define BGM_BEGIN 16'h0
`define BGM_END 16'hAFEA
/*
BGM             Dea               FB jump         WG jump
0x0000-0xAFEA   0xAFEB-0xC554    0xC555-0xCADD   0xCADE-0xD5BE

audio_ctrl
bgm-start, 2:0 is sound selection
*/
module audio_play(input logic        clk,
                input logic 	   reset,

                input left_chan_ready,
                input right_chan_ready,

                input logic [2:0] audio_ctrl,

                output logic [15:0] sample_data_l,
                output logic sample_valid_l,
                output logic [15:0] sample_data_r,
                output logic sample_valid_r);

    logic [15:0] sound_begin_addresses [3:0] = '{16'h0 ,16'hAFEB, 16'hc555, 16'hcade};
    logic [15:0] sound_end_addresses [3:0] = '{16'h0 ,16'hc554, 16'hcadd, 16'hd5be};

    logic [15:0] sound_address;
    logic [15:0] sound_end_address;
    logic [15:0] bgm_address; // loop
    logic [7:0] sound_data;
    logic [7:0] bgm_data;
    
    logic left_busy;
    logic right_busy;

    logic bgm_playing;
    logic sfx_playing;
    
    logic [2:0] audio_ctrl_prev;

    audio_rom u_audio_rom(
        .address_a 	(bgm_address),
        .address_b  (sound_address),
        .clock   	(clk    ),
        .q_a       	(bgm_data),
        .q_b        (sound_data)
    );
    
    

    assign sample_data_l = bgm_data << 8;
    assign sample_data_r = sound_data << 8;

    always_ff @(posedge clk) begin
        if (reset) begin
            sample_valid_l <= 0;
            sample_valid_r <= 0;
            left_busy <= 0;
            right_busy <= 0;
            sound_address <= `BGM_BEGIN;
            bgm_address <= `BGM_BEGIN;
            sound_end_address <= `BGM_END;
            bgm_playing <= 0;
            sfx_playing <= 0;
            audio_ctrl_prev <= 0;
        end
        else begin
            if (audio_ctrl[2] != audio_ctrl_prev[2]) begin
                audio_ctrl_prev[2] <= audio_ctrl[2];
                if (audio_ctrl[2]) begin
                    bgm_playing <= 1;
                    bgm_address <= `BGM_BEGIN;
                    if(!sfx_playing) begin
                        sound_address <= `BGM_BEGIN;
                        sound_end_address <= `BGM_END;
                    end
                end else begin
                    bgm_playing <= 0;
                end           
            end
            if (audio_ctrl[1:0] != audio_ctrl_prev[1:0]) begin
                audio_ctrl_prev[1:0] <= audio_ctrl[1:0];
                if (audio_ctrl[1:0] !=0) begin
                    sound_address <= sound_begin_addresses[audio_ctrl[1:0]];
                    sound_end_address <= sound_end_addresses[audio_ctrl[1:0]];
                    sfx_playing <= 1;
                end begin
                    sfx_playing <= 0;
                end    
            end
            
            
            if (bgm_playing || sfx_playing) begin
                if (left_chan_ready == 1 && right_chan_ready == 1) begin // our fpga clock is much faster than sampling rate
                    if(left_busy == 0 && right_busy == 0) begin // only feed data when audio is ready
                        // current sound ends
                        if(sound_address >= sound_end_address) begin
                            // if sound effect ends, continue playing bgm
                            if(sfx_playing) begin
                                sound_address <= bgm_address;
                                sound_end_address <= `BGM_END;
                                sfx_playing <= 0;
                                // repeat bgm
                            end
                            else begin
                                sound_address <= `BGM_BEGIN;
                                sound_end_address <= `BGM_END;
                            end
                        end
                        else begin
                            sound_address <= sound_address + 1;
                        end
                        
                        if (bgm_address >= `BGM_END) begin
                            bgm_address <= `BGM_BEGIN;
                        end else begin
                            bgm_address <= bgm_address + 1;
                        end
                         // if(sound_address >= sound_end_address)
                    end // if(left_busy == 0 && right_busy == 0)

                    left_busy <= 1;
                    right_busy <= 1;
                    sample_valid_l <= 1;
                    sample_valid_r <= 1;
                end
                else if (left_chan_ready == 0 && right_chan_ready == 0) begin // wait until ready becomes 0
                    left_busy <= 0;
                    right_busy <= 0;
                    sample_valid_l <= 0;
                    sample_valid_r <= 0;
                end
            end
            else begin
                sample_valid_l <= 0;
                sample_valid_r <= 0;
            end // if (left_chan_ready == 1 && right_chan_ready == 1)

        end 



    end // if (bgm_playing || sfx_playing)

endmodule


