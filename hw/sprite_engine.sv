// sprite_attr
/*
[31] : Enable = 1, Disable = 0
[30] : Flip = 1, otherise = 0
[29] : Reserved
[28:27] : Priority (0-3)
[26:18] sprite_pos_row (0-479)
[17:8] : sprite_pos_col (0-639)
[7:0] : frame_id (0-255)

*/

module sprite_engine(
    input logic clk,
    input logic reset,
    input logic sprite_start,
    input logic sprite_wren,
    input logic [4:0] sprite_idx,
    input logic [31:0] sprite_attr,
    input logic [9:0] vcount,
    output logic [9:0] sprite_pixel_col, // addr_pixel_draw
    output logic [15:0] sprite_pixel_data, // data_pixel_draw
    output logic sprite_done,
    output logic wren_pixel_draw
);
endmodule