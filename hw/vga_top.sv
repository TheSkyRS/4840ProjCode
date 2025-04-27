module vga_top(input logic        clk,
                   input logic 	   reset,
                   input logic [31:0]  writedata,
                   input logic 	   write,
                   input 		   chipselect,
                   input logic [5:0]  address, // 64

                   output logic [31:0] readdata,
                   output logic [7:0] VGA_R, VGA_G, VGA_B,
                   output logic 	   VGA_CLK, VGA_HS, VGA_VS,
                   VGA_BLANK_n,
                   output logic 	   VGA_SYNC_n);

    // current VGA pixel coord
    logic [10:0]	   hcount;
    logic [9:0]     vcount;

    logic[31:0] status_reg;
    logic[31:0] ctrl_reg;

	// tile engine
	logic tile_start;
	// output declaration of module tile_engine
	logic[5:0] tile_col;
	logic [255:0] tile_data;
	logic tile_done;
	
	tile_engine u_tile_engine(
		.clk         	(clk          ),
		.reset       	(reset        ),
		.tile_start  	(tile_start   ),
		.tilemap_idx 	(ctrl_reg[1:0]  ),
		.vcount      	(vcount       ),
		.tile_col    	(tile_col     ),
		.tile_data   	(tile_data    ),
		.tile_done   	(tile_done    )
	);
	

	// sprite engine
    logic sprite_start;
    logic sprite_done;

    parameter TILE_SIZE = 16;

    vga_counters counters(.clk50(clk), .*);

    always_ff @(posedge clk) begin
        if (reset) begin
			status_reg <= 0;
			ctrl_reg <= 0;
			tile_start <= 0;
			sprite_start <= 0;

        end
        else begin
            if (chipselect) begin
                if (write) begin
                    case (address)
                        5'h0: ctrl_reg <= writedata;
                    endcase
                end
                else begin // read
                    case (address)
                        5'h1: readdata <= status_reg;
                    endcase
                end
            end

        end
    end



    always_comb begin
        {VGA_R, VGA_G, VGA_B} = 24'h000000;
        if (VGA_BLANK_n) begin
            // TODO
            logic signed [11:0] dx, dy;
            dx = (hcount >> 1) - (ball_x_eff[10:1] + BALL_SIZE/2);
            dy = vcount - (ball_y_eff + BALL_SIZE/2);

            if ((dx*dx + dy*dy) <= ((BALL_SIZE/2)*(BALL_SIZE/2)))
                {VGA_R, VGA_G, VGA_B} = 24'hffffff;
            else
                {VGA_R, VGA_G, VGA_B} = {background_r, background_g, background_b};
        end
    end

endmodule

module vga_counters(
        input logic 	     clk50, reset,
        output logic [10:0] hcount,  // hcount[10:1] is pixel column
        output logic [9:0]  vcount,  // vcount[9:0] is pixel row
        output logic 	     VGA_CLK, VGA_HS, VGA_VS, VGA_BLANK_n, VGA_SYNC_n);

    /*
     * 640 X 480 VGA timing for a 50 MHz clock: one pixel every other cycle
     * 
     * HCOUNT 1599 0             1279       1599 0
     *             _______________              ________
     * ___________|    Video      |____________|  Video
     * 
     * 
     * |SYNC| BP |<-- HACTIVE -->|FP|SYNC| BP |<-- HACTIVE
     *       _______________________      _____________
     * |____|       VGA_HS          |____|
     */
    // Parameters for hcount
    parameter HACTIVE      = 11'd 1280,
              HFRONT_PORCH = 11'd 32,
              HSYNC        = 11'd 192,
              HBACK_PORCH  = 11'd 96,
              HTOTAL       = HACTIVE + HFRONT_PORCH + HSYNC +
              HBACK_PORCH; // 1600

    // Parameters for vcount
    parameter VACTIVE      = 10'd 480,
              VFRONT_PORCH = 10'd 10,
              VSYNC        = 10'd 2,
              VBACK_PORCH  = 10'd 33,
              VTOTAL       = VACTIVE + VFRONT_PORCH + VSYNC +
              VBACK_PORCH; // 525

    logic endOfLine;

    always_ff @(posedge clk50 or posedge reset)
        if (reset)
            hcount <= 0;
        else if (endOfLine)
            hcount <= 0;
        else
            hcount <= hcount + 11'd 1;

    assign endOfLine = hcount == HTOTAL - 1;

    logic endOfField;

    always_ff @(posedge clk50 or posedge reset)
        if (reset)
            vcount <= 0;
        else if (endOfLine)
            if (endOfField)
                vcount <= 0;
            else
                vcount <= vcount + 10'd 1;

    assign endOfField = vcount == VTOTAL - 1;

    // Horizontal sync: from 0x520 to 0x5DF (0x57F)
    // 101 0010 0000 to 101 1101 1111
    assign VGA_HS = !( (hcount[10:8] == 3'b101) &
                       !(hcount[7:5] == 3'b111));
    assign VGA_VS = !( vcount[9:1] == (VACTIVE + VFRONT_PORCH) / 2);

    assign VGA_SYNC_n = 1'b0; // For putting sync on the green signal; unused

    // Horizontal active: 0 to 1279     Vertical active: 0 to 479
    // 101 0000 0000  1280	       01 1110 0000  480
    // 110 0011 1111  1599	       10 0000 1100  524
    assign VGA_BLANK_n = !( hcount[10] & (hcount[9] | hcount[8]) ) &
           !( vcount[9] | (vcount[8:5] == 4'b1111) );

    /* VGA_CLK is 25 MHz
     *             __    __    __
     * clk50    __|  |__|  |__|
     *        
     *             _____       __
     * hcount[0]__|     |_____|
     */
    assign VGA_CLK = hcount[0]; // 25 MHz clock: rising edge sensitive

endmodule
