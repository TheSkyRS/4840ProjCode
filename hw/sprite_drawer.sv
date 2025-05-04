module sprite_drawer (
    input  logic        clk,
    input  logic        reset,

    // 启动参数
    input  logic        start,          // 1 clk 脉冲
    input  logic [9:0]  col_base,
    input  logic        flip,
    input  logic [7:0]  frame_id,
    input  logic [3:0]  row_off,

    // ROM
    output logic [15:0] rom_addr,
    input  logic [15:0] rom_q,

    // linebuffer 写口
    output logic [9:0]  pixel_col,
    output logic [15:0] pixel_data,
    output logic        wren,

    output logic        busy,
    output logic        done            // 1 clk pulse at last pixel
);
    // idx  0‥15 : 正在发地址
    // idx_d 1‥16: 与 rom_q 对齐的数据拍
    logic [4:0] idx;          // 0‑15
    logic [4:0] idx_d;        // 延迟 1 拍
    logic       valid_d;      // 延迟 1 拍的有效信号

    logic [9:0] col_now;
    assign col_now = flip ? (col_base + (10'd15 - idx_d)) :
                        (col_base + idx_d);
    // === 时序 ===========================================================
    always_ff @(posedge clk) begin
        if (reset) begin
            busy  <= 0;
            done  <= 1;
            wren  <= 0;
            idx   <= 0;
            idx_d <= 0;
            valid_d <= 0;
            rom_addr <= 0;
        end else begin
            // ---------- 启动 --------------------------------------------
            if (start) begin
                done  <= 0;                  // 默认拉低
                wren  <= 0;                  // 默认不写
                valid_d <= 0;                // 默认无数据
                busy     <= 1;
                idx      <= 0;
                rom_addr <= {frame_id, 8'b0} + {row_off, 4'b0};
            end
            // ---------- 发地址阶段 --------------------------------------
            else if (busy) begin
                // pipeline: 本拍送地址 idx，上一拍地址的数据现在有效
                rom_addr <= rom_addr + 16'd1;
                idx      <= idx + 5'd1;
                valid_d  <= 1;           // 下一拍使能写

                // 保存延迟计数
                idx_d   <= idx;

                // 当最后一个地址发完，下一拍收到最后一个像素后 busy 结束
                if (idx == 5'd15) begin
                    busy <= 0;           // 但仍有最后一拍数据输出
                end
            end

            // ---------- 数据对齐后输出 ----------------------------------
            if (valid_d) begin
                pixel_col  <= col_now;
                pixel_data <= rom_q;
                wren       <= (rom_q[15] == 1'b0); // 最高位=0 ⇒ 非透明写
            end

            // ---------- 绘制完成标志 -----------------------------------
            if (valid_d && (idx_d == 5'd15)) begin
                done <= 1'b1;            // 最后一个像素那拍
            end
        end
    end
endmodule
