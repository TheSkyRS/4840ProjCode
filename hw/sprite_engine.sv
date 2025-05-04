/* sprite_attr
[31] : Enable = 1, Disable = 0
[30] : Flip = 1, otherise = 0
[29] : Reserved
[28:27] : Priority (0-3) (0 is the highest prioirty)
[26:18] sprite_pos_row (0-479)
[17:8] : sprite_pos_col (0-639)
[7:0] : frame_id (0-255)

*/

// ---------------------------------------------------------------------------
//  sprite_engine
//  · 顶层只暴露：sprite_start / vcount / 属性写口 / linebuffer 写口 / done
//  · 内部例化：attr_ram ×1，sprite_frontend ×1，sprite_drawer ×1，sprite_rom ×1
// ---------------------------------------------------------------------------
module sprite_engine #(
    parameter NUM_SPRITE = 32,
    parameter MAX_SLOT   = 8
)(
    input  logic        clk,
    input  logic        reset,

    // 行开始脉冲：顶层在写缓冲换行时拉高 1 clk
    input  logic        sprite_start,

    // 当前正在显示的行号 0‑524（用于 next_vcount = vcount+1）
    input  logic [9:0]  vcount,

    // -------- 属性写口 (32×32‑bit) ------------------------------------
    input  logic                            spr_wr_en,
    input  logic [$clog2(NUM_SPRITE)-1:0]   spr_wr_idx,
    input  logic [31:0]                     spr_wr_data,

    // -------- linebuffer 写口 -----------------------------------------
    output logic [9:0]  sprite_pixel_col,
    output logic [15:0] sprite_pixel_data,
    output logic        wren_pixel_draw,

    // 行完成：前端已扫完一轮且 drawer 空闲
    output logic        done
);
    // ------------------- next_vcount 计算 ------------------------------
    logic [9:0] next_vcount;
    assign next_vcount = (vcount < 10'd479) ? vcount + 10'd1 :
                         (vcount == 10'd524) ? 10'd0      : vcount;

    // ------------------- 属性 RAM (写+同步读) --------------------------
    logic [31:0] attr_rd;
    logic [$clog2(NUM_SPRITE)-1:0] attr_ra;

    sprite_attr_ram #(.NUM_SPRITE(NUM_SPRITE)) u_ram (
        .clk   (clk),
        .ra    (attr_ra),
        .q     (attr_rd),
        .write (spr_wr_en),
        .wa    (spr_wr_idx),
        .d     (spr_wr_data)
    );

    // ------------------- 前端 -----------------------------------------
    logic fe_draw_req, fe_flip, fe_busy, fe_done;
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
        .draw_req   (fe_draw_req),
        .col_base   (fe_col),
        .flip       (fe_flip),
        .frame_id   (fe_frame),
        .row_off    (fe_rowoff),
        .busy       (fe_busy),
        .fe_done    (fe_done)
    );

    // ------------------- ROM ------------------------------------------
    logic [15:0] rom_addr, rom_q;
    sprite_pattern_rom u_rom (.clk(clk), .addr(rom_addr), .q(rom_q));

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
        .busy      (fe_busy),
        .done      ()               // 未用
    );

    // ------------------- 行完成输出 -----------------------------------
    assign done = fe_done && !fe_busy;

endmodule
