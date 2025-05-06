/* sprite_attr
[31] : Enable = 1, Disable = 0
[30] : Flip = 1, otherise = 0
[29: 27]: Reserved
[26:18] sprite_pos_row (0-479)
[17:8] : sprite_pos_col (0-639)
[7:0] : frame_id (0-255)
*/

// Test example: 1000 0100 0000 0001 0000 0000 0000 0001
//               8      4   0   1       0   0   0   1
module sprite_engine #(
    parameter NUM_SPRITE = 32,
    parameter MAX_SLOT   = 8
)(
    input  logic        clk,
    input  logic        reset,

    input  logic        sprite_start,

    input  logic [9:0]  vcount,

    input  logic                            spr_wr_en,
    input  logic [$clog2(NUM_SPRITE)-1:0]   spr_wr_idx,
    input  logic [31:0]                     spr_wr_data,

    output logic [9:0]  sprite_pixel_col,
    output logic [15:0] sprite_pixel_data,
    output logic        wren_pixel_draw,

    output logic        done
);
    logic [9:0] next_vcount;
    assign next_vcount = (vcount < 10'd479) ? vcount + 10'd1 :
                         (vcount == 10'd524) ? 10'd0      : vcount + 1;

    logic [31:0] attr_rd;
    logic [$clog2(NUM_SPRITE)-1:0] attr_ra;

    // sprite_attr_ram #(.NUM_SPRITE(NUM_SPRITE)) u_ram (
    //     .clk   (clk),
    //     .ra    (attr_ra),
    //     .q     (attr_rd),
    //     .write (spr_wr_en),
    //     .wa    (spr_wr_idx),
    //     .d     (spr_wr_data)
    // );

    logic [31:0] sprite_attr_ram [NUM_SPRITE];
    always_ff @(posedge clk) begin
        // if (spr_wr_en) begin
        //     sprite_attr_ram[spr_wr_idx] <= spr_wr_data;
        // end
        attr_rd <= sprite_attr_ram[attr_ra];
    end

    // Test exmaple:
    // assign sprite_attr_ram[0] = 32'h83010001;
    // assign sprite_attr_ram[1] = 32'h90F14001;
    // assign sprite_attr_ram[2] = 32'h84016001;
    // assign sprite_attr_ram[3] = 32'h91F18001;
    // Test exmaple:
    // assign sprite_attr_ram[0] = 32'h84010001;
    // assign sprite_attr_ram[1] = 32'h90F14001;
    // assign sprite_attr_ram[2] = 32'h04016001;
    // assign sprite_attr_ram[3] = 32'h90F18001;

    // Test Example: distruibuted equally on the entire screen
    // assign sprite_attr_ram[0]  = 32'h80D02000;
    // assign sprite_attr_ram[1]  = 32'h80D07001;
    // assign sprite_attr_ram[2]  = 32'h80D0C002;
    // assign sprite_attr_ram[3]  = 32'h80D11003;
    // assign sprite_attr_ram[4]  = 32'h80D16004;
    // assign sprite_attr_ram[5]  = 32'h80D1B005;
    // assign sprite_attr_ram[6]  = 32'h80D20006;
    // assign sprite_attr_ram[7]  = 32'h80D25007;
    // assign sprite_attr_ram[8]  = 32'h82B02008;
    // assign sprite_attr_ram[9]  = 32'h82B07009;
    // assign sprite_attr_ram[10] = 32'h82B0C00A;
    // assign sprite_attr_ram[11] = 32'h82B1100B;
    // assign sprite_attr_ram[12] = 32'h82B1600C;
    // assign sprite_attr_ram[13] = 32'h82B1B00D;
    // assign sprite_attr_ram[14] = 32'h82B2000E;
    // assign sprite_attr_ram[15] = 32'h82B2500F;
    // assign sprite_attr_ram[16] = 32'h84902010;
    // assign sprite_attr_ram[17] = 32'h84907011;
    // assign sprite_attr_ram[18] = 32'h8490C012;
    // assign sprite_attr_ram[19] = 32'h84911013;
    // assign sprite_attr_ram[20] = 32'h84916014;
    // assign sprite_attr_ram[21] = 32'h8491B015;
    // assign sprite_attr_ram[22] = 32'h84920016;
    // assign sprite_attr_ram[23] = 32'h84925017;
    // assign sprite_attr_ram[24] = 32'h86702018;
    // assign sprite_attr_ram[25] = 32'h86707019;
    // assign sprite_attr_ram[26] = 32'h8670C01A;
    // assign sprite_attr_ram[27] = 32'h8671101B;
    // assign sprite_attr_ram[28] = 32'h8671601C;
    // assign sprite_attr_ram[29] = 32'h8671B01D;
    // assign sprite_attr_ram[30] = 32'h8672001E;
    // assign sprite_attr_ram[31] = 32'h8672501F;

    // Diagonal Sprite Positions (leftup to rightdown)
    assign sprite_attr_ram[0]  = 32'h80000000;
    assign sprite_attr_ram[1]  = 32'h80381401;
    assign sprite_attr_ram[2]  = 32'h80702802;
    assign sprite_attr_ram[3]  = 32'h80A83C03;
    assign sprite_attr_ram[4]  = 32'h80E05004;
    assign sprite_attr_ram[5]  = 32'h81186405;
    assign sprite_attr_ram[6]  = 32'h81507806;
    assign sprite_attr_ram[7]  = 32'h81888C07;
    assign sprite_attr_ram[8]  = 32'h81C0A008;
    assign sprite_attr_ram[9]  = 32'h81F8B409;
    assign sprite_attr_ram[10] = 32'h8230C80A;
    assign sprite_attr_ram[11] = 32'h8268DC0B;
    assign sprite_attr_ram[12] = 32'h82A0F00C;
    assign sprite_attr_ram[13] = 32'h82D9040D;
    assign sprite_attr_ram[14] = 32'h8311180E;
    assign sprite_attr_ram[15] = 32'h83492C0F;
    assign sprite_attr_ram[16] = 32'h83814010;
    assign sprite_attr_ram[17] = 32'h83B95411;
    assign sprite_attr_ram[18] = 32'h83F16812;
    assign sprite_attr_ram[19] = 32'h84297C13;
    assign sprite_attr_ram[20] = 32'h84619014;
    assign sprite_attr_ram[21] = 32'h8499A415;
    assign sprite_attr_ram[22] = 32'h84D1B816;
    assign sprite_attr_ram[23] = 32'h8509CC17;
    assign sprite_attr_ram[24] = 32'h8541E018;
    assign sprite_attr_ram[25] = 32'h8579F419;
    assign sprite_attr_ram[26] = 32'h85B2081A;
    assign sprite_attr_ram[27] = 32'h85EA1C1B;
    assign sprite_attr_ram[28] = 32'h8622301C;
    assign sprite_attr_ram[29] = 32'h865A441D;
    assign sprite_attr_ram[30] = 32'h8692581E;
    assign sprite_attr_ram[31] = 32'h86CA6C1F;

    // ------------------- 前端 -----------------------------------------
    logic fe_draw_req, fe_flip, fe_done;
    logic dw_done;
    logic [9:0] fe_col;
    logic [7:0] fe_frame;
    logic [3:0] fe_rowoff;

    sprite_frontend #(
        .NUM_SPRITE (NUM_SPRITE),
        .MAX_SLOT   (MAX_SLOT)
    ) u_fe (
        .clk        (clk),
        .reset      (reset),
        .start_row  (sprite_start),
        .next_vcount(next_vcount),
        .ra         (attr_ra),
        .rd_data    (attr_rd),
        .draw_done (dw_done),
        .draw_req   (fe_draw_req),
        .col_base   (fe_col),
        .flip       (fe_flip),
        .frame_id   (fe_frame),
        .row_off    (fe_rowoff),
        .fe_done    (fe_done)
    );

    // ------------------- ROM ------------------------------------------
    logic [15:0] rom_addr, rom_q;
    sprite_pattern_rom u_rom (.clock(clk), .address(rom_addr), .q(rom_q));

    // ------------------- Drawer ---------------------------------------
    sprite_drawer u_dw (
        .clk       (clk),
        .reset     (reset),
        .start     (fe_draw_req),
        .col_base  (fe_col),
        .flip      (fe_flip),
        .frame_id  (fe_frame),
        .row_off   (fe_rowoff),
        .rom_addr  (rom_addr),
        .rom_q     (rom_q),
        .pixel_col (sprite_pixel_col),
        .pixel_data(sprite_pixel_data),
        .wren      (wren_pixel_draw),
        .done      (dw_done)
    );

    // ------------------- 行完成输出 -----------------------------------
    assign done = (fe_done) || (vcount >= 479 && vcount < 524);

endmodule
