`timescale 1ns / 1ps

module tb_sprite_engine;

    // ---------------------------
    // 信号声明（与 DUT 接口匹配）
    // ---------------------------
    logic clk;
    logic reset;

    logic sprite_start;
    logic [9:0] vcount;
    logic spr_wr_en;
    logic [4:0] spr_wr_idx;
    logic [31:0] spr_wr_data;

    logic [4:0] debug_addr;
    logic [31:0] debug_data;

    logic [9:0] sprite_pixel_col;
    logic [15:0] sprite_pixel_data;
    logic wren_pixel_draw;
    logic done;

    // ---------------------------
    // 实例化 DUT（待测试模块）
    // ---------------------------
    sprite_engine u_dut (
        .clk(clk),
        .reset(reset),
        .sprite_start(sprite_start),
        .vcount(vcount),
        .spr_wr_en(spr_wr_en),
        .spr_wr_idx(spr_wr_idx),
        .spr_wr_data(spr_wr_data),
        .debug_addr(debug_addr),
        .debug_data(debug_data),
        .sprite_pixel_col(sprite_pixel_col),
        .sprite_pixel_data(sprite_pixel_data),
        .wren_pixel_draw(wren_pixel_draw),
        .done(done)
    );

    // ---------------------------
    // 生成时钟：10ns周期 = 100MHz
    // ---------------------------
    always #5 clk = ~clk;

    // ---------------------------
    // 主测试过程
    // ---------------------------
    initial begin
        // 初始值
        clk = 0;
        reset = 1;
        sprite_start = 0;
        vcount = 0;

        spr_wr_en = 0;
        spr_wr_idx = 0;
        spr_wr_data = 0;

        debug_addr = 0;

        // 保持复位
        #20;
        reset = 0;

        // 写入 index = 5，数据 = 0xABCD1234
        #10;
        spr_wr_en = 1;
        spr_wr_idx = 5;
        spr_wr_data = 32'hABCD1234;

        #10;
        spr_wr_en = 0;

        // 改变 debug_addr 来观察 index 5 的写入数据
        #10;
        debug_addr = 5;

        // 写入 index = 8，数据 = 0xDEADBEEF
        #10;
        spr_wr_en = 1;
        spr_wr_idx = 8;
        spr_wr_data = 32'hDEADBEEF;

        #10;
        spr_wr_en = 0;

        #10;
        debug_addr = 8;

        // 等待观察
        #100;

        $stop;
    end

endmodule