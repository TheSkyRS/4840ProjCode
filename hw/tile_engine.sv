module tile_engine(
    input logic clk,
    input logic reset,
    input logic tile_start,
    input logic[1:0] tilemap_idx,
    input logic[9:0] vcount,
    output logic[5:0] tile_col,
    output logic [255:0] tile_data,
    output logic tile_done

);
    
endmodule