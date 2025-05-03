module tile_engine(
    input logic clk,
    input logic reset,
    input logic tile_start,
    input logic [1:0] tilemap_idx,
    input logic [9:0] vcount,
    output logic [5:0] tile_col,
    output logic [255:0] tile_data,
    output logic tile_done,
    output logic wren_tile_draw
);

// internal
logic [11:0] tilemap_addr;
logic [11:0] tile_pattern_addr;


logic [7:0] tile_id;
logic [9:0] next_vcount;
logic [5:0] col[4:0];
assign tile_col = col[4];

assign wren_tile_draw = (!tile_done) && (col[0] > 0);

assign tilemap_addr = tilemap_idx * 1200 +  (next_vcount >> 4) * 40 + col[0];
tilemap u_tilemap(
    .address 	(tilemap_addr  ),
    .clock   	(clk),
    .q       	(tile_id)
);

assign tile_pattern_addr = (tile_id << 4) + next_vcount[3:0];
tile_pattern u_tile_pattern(
    .address 	(tile_pattern_addr  ),
    .clock   	(clk),
    .q       	(tile_data)
);

always_ff @(posedge clk) begin
    if (reset) begin
        col[0] <= 0;
        col[1] <= 0;
        col[2] <= 0;
        col[3] <= 0;
        col[4] <= 0;
        tile_done <= 1;
    end else begin
        if (tile_start) begin
            col[0] <= 0;
            col[1] <= 0;
            col[2] <= 0;
            col[3] <= 0;
            col[4] <= 0;
            if (vcount < 479) begin
                next_vcount <= vcount + 1;
                tile_done <= 0;
            end else if (vcount >= 479 && vcount < 524) begin
                tile_done <= 1;
            end else if (vcount == 524) begin
                next_vcount <= 0;
                tile_done <= 0;
            end
        end else if (!tile_done) begin
            col[1] <= col[0];
            col[2] <= col[1];
            col[3] <= col[2];
            col[4] <= col[3];
            if (col[0] < 39) begin
                col[0] <= col[0] + 1;
            end
            if (col[4] == 39) begin
                tile_done <= 1;
            end
        end
    end
end
    
endmodule

// import "DPI-C" function void tilemap_read(input int addr, output byte q);
// import "DPI-C" function void tile_pattern_read(input int addr, output shortint pixels[16]);

// module tilemap(input logic [11:0] address, input logic clock, output logic [7:0] q);
//     always_ff @(posedge clock) tilemap_read(address, q);
// endmodule

// module tile_pattern(input logic [11:0] address, input logic clock, output logic [255:0] q);
//     shortint pixels[16];
//     always_ff @(posedge clock) begin
//         tile_pattern_read(address, pixels);
//         q <= {pixels[0], pixels[1], pixels[2], pixels[3],
//               pixels[4], pixels[5], pixels[6], pixels[7],
//               pixels[8], pixels[9], pixels[10], pixels[11],
//               pixels[12], pixels[13], pixels[14], pixels[15]};
//     end
// endmodule