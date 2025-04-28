module linebuffer(
    input logic clk,
    input logic reset,
    input logic switch,
    input logic [5:0] addr_tile_disp,
    input logic [9:0] addr_pixel_disp,
    input logic [5:0] addr_tile_draw,
    input logic [9:0] addr_pixel_draw,

    // indata
    input logic [255:0] data_tile_disp,
    input logic [15:0]  data_pixel_disp,
    input logic [255:0] data_tile_draw,
    input logic [15:0]  data_pixel_draw,

    // wren
    input logic wren_tile_disp,
    input logic wren_pixel_disp,
    input logic wren_tile_draw,
    input logic wren_pixel_draw,

    // outdata
    output logic [255:0] q_tile_disp,
    output logic [15:0]  q_pixel_disp,
    output logic [255:0] q_tile_draw,
    output logic [15:0]  q_pixel_draw
);

    logic [5:0] addr_tile[1:0];
    logic [9:0] addr_pixel[1:0];

    logic [255:0] data_tile[1:0];
    logic [15:0]  data_pixel[1:0];

    logic wren_tile[1:0];
    logic wren_pixel[1:0];

    logic [255:0] q_tile[1:0];
    logic [15:0]  q_pixel[1:0];
    
    linebuffer_ram linebuffer_ram0(
        .address_a 	(addr_tile[0] ),
        .address_b 	(addr_pixel[0] ),
        .clock     	(clk      ),
        .data_a    	(data_tile[0] ),
        .data_b    	(data_pixel[0] ),
        .wren_a    	(wren_tile[0]  ),
        .wren_b    	(wren_pixel[0] ),
        .q_a       	(q_tile[0]     ),
        .q_b       	(q_pixel[0]     )
    );

    linebuffer_ram linebuffer_ram1(
        .address_a 	(addr_tile[1] ),
        .address_b 	(addr_pixel[1] ),
        .clock     	(clk      ),
        .data_a    	(data_tile[1] ),
        .data_b    	(data_pixel[1] ),
        .wren_a    	(wren_tile[1]  ),
        .wren_b    	(wren_pixel[1] ),
        .q_a       	(q_tile[1]     ),
        .q_b       	(q_pixel[1]     )
    );
    
    always_comb begin
        if (switch) begin
            // RAM0 disp, RAM1 draw
            addr_tile[0] = addr_tile_disp;
            addr_pixel[0] = addr_pixel_disp;
            data_tile[0] = data_tile_disp;
            data_pixel[0] = data_pixel_disp;
            wren_tile[0] = wren_tile_disp;
            wren_pixel[0] = wren_pixel_disp;

            addr_tile[1] = addr_tile_draw;
            addr_pixel[1] = addr_pixel_draw;
            data_tile[1] = data_tile_draw;
            data_pixel[1] = data_pixel_draw;
            wren_tile[1] = wren_tile_draw;
            wren_pixel[1] = wren_pixel_draw;
        end else begin
            // RAM0 draw, RAM1 disp
            addr_tile[1] = addr_tile_disp;
            addr_pixel[1] = addr_pixel_disp;
            data_tile[1] = data_tile_disp;
            data_pixel[1] = data_pixel_disp;
            wren_tile[1] = wren_tile_disp;
            wren_pixel[1] = wren_pixel_disp;

            addr_tile[0] = addr_tile_draw;
            addr_pixel[0] = addr_pixel_draw;
            data_tile[0] = data_tile_draw;
            data_pixel[0] = data_pixel_draw;
            wren_tile[0] = wren_tile_draw;
            wren_pixel[0] = wren_pixel_draw;
        end
    end
    always_ff @(posedge clk) begin
        q_tile_disp <= switch ? q_tile[0] : q_tile[1];
        q_pixel_disp <= switch ? q_pixel[0] : q_pixel[1];
        
        q_tile_draw = switch ? q_tile[1] : q_tile[0];
        q_pixel_draw = switch ? q_pixel[1] : q_pixel[0];
    end
    
endmodule