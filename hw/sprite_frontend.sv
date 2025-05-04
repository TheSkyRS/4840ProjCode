// ---------------------------------------------------------------------------
//  sprite_frontend.sv
//  - 行开始后持续扫描属性表，命中 sprite 进入 8‑slot 队列
//  - slot 计数到 0 且 drawer 空闲时，按 Priority 发送 draw_req
// ---------------------------------------------------------------------------
module sprite_frontend #(
    parameter int NUM_SPRITE = 32,
    parameter int MAX_SLOT   = 8,
    localparam int IDXW      = $clog2(NUM_SPRITE),
    localparam int PTRW      = $clog2(MAX_SLOT)
)(
    input  logic               clk,
    input  logic               reset,

    // 行开始 & 行号
    input  logic               start_row,
    input  logic [9:0]         next_vcount,     // vcount + 1 (0‑479)

    // --- 属性 RAM 端口 (同步读 1 clk) ------------------------------------
    output logic [IDXW-1:0]    ra,      // 读地址
    input  logic [31:0]        rd_data, // 1 clock cycle delay

    // --- 与 drawer 的握手 ----------------------------------------------
    input  logic               busy,    // drawer 正在写 16px
    output logic               draw_req,
    output logic [9:0]         col_base,
    output logic               flip,
    output logic [7:0]         frame_id,
    output logic [3:0]         row_off,

    // 行完成
    output logic               fe_done
);
    // --------------------- slot 结构 ------------------------------------
    typedef struct packed {
        logic        valid;
        logic [9:0]  cnt;      // down-counter
        logic [1:0]  pri;
        logic        flip;
        logic [7:0]  frame;
        logic [9:0]  col;      // 固定左列
        logic [3:0]  rowoff;   // 行内偏移
    } slot_t;
    slot_t slot [MAX_SLOT];

    // --------------------- 控制寄存器 -----------------------------------
    logic [IDXW-1:0] scan_idx;
    logic            scan_complete;
    logic            hit_d, enable_d;
    logic [9:0]      col_d;
    logic [1:0]      pri_d;
    logic            flip_d;
    logic [7:0]      frame_d;
    logic [3:0]      rowoff_d;
    logic [PTRW-1:0] slot_ptr;
    logic [PTRW-1:0] clear_slot;
    logic clear_pending;
    // --------------------- sprite_check (组合) --------------------------
    always_comb begin
        hit_d    = rd_data[31] &&
                   next_vcount >= rd_data[26:18] &&
                   next_vcount <  rd_data[26:18] + 16;
        col_d    = rd_data[17:8];
        pri_d    = rd_data[28:27];
        flip_d   = rd_data[30];
        frame_d  = rd_data[7:0];
        rowoff_d = next_vcount - rd_data[26:18];
    end
    logic tmp_any;        // 组合临时
                    
    always_comb begin
        tmp_any = 1'b0;
        for (int k = 0; k < MAX_SLOT; k++)
            tmp_any |= slot[k].valid;
    end


    // --------------------- 仲裁 (组合) -----------------------------------
    logic                   arb_found;
    logic [PTRW-1:0]        arb_slot;
    logic [1:0]             arb_pri;

    always_comb begin
        arb_found = 1'b0;
        arb_pri   = 2'b11;
        arb_slot  = '0;
        for (int j = 0; j < MAX_SLOT; j++) begin
            if (slot[j].valid && slot[j].cnt == 0) begin
                if (!arb_found || slot[j].pri < arb_pri) begin
                    arb_found = 1'b1;
                    arb_pri   = slot[j].pri;
                    arb_slot  = j;
                end
            end
        end
    end

    // --------------------- 主时序 ---------------------------------------
    logic any_v;
    integer i;
    always_ff @(posedge clk) begin
        if (reset) begin
            for (i=0;i<MAX_SLOT;i++) slot[i].valid <= 0;
            scan_idx <= 0; ra <= 0;
            slot_ptr <= 0; scan_complete <= 0;
            draw_req <= 0; fe_done <= 1;
            any_v <= 0; 
            clear_pending <= 0;
            clear_slot <= 0;
        end else begin
            // ===== 行开始：复位 front‑end 状态 ==========================
            if (start_row) begin
                for (i=0;i<MAX_SLOT;i++) slot[i].valid <= 0;
                slot_ptr      <= 0;
                scan_idx      <= 0;
                ra            <= 0;
                scan_complete <= 0;
                fe_done       <= 0;
                draw_req <= 0;   
                clear_pending <= 0;
            end else if (!fe_done) begin
                // ===== 属性扫描，每拍前推索引 ==============================
                // 无论 slot 是否已满，都循环扫描；只有 slot_ptr<MAX_SLOT 才登记
                ra <= scan_idx;
                if (scan_idx == NUM_SPRITE-1) begin
                    scan_idx      <= 0;
                    scan_complete <= 1;
                end else
                    scan_idx <= scan_idx + 1;

                // hit && slot 未满 ⇒ 写入 slot
                if (hit_d && slot_ptr < MAX_SLOT) begin
                    slot[slot_ptr] <= '{1, col_d, pri_d, flip_d, frame_d, col_d, rowoff_d};
                    slot_ptr       <= slot_ptr + 1;
                end

                // ---------- down‑counter 递减 ----------
                for (i = 0; i < MAX_SLOT; i++)
                    if (slot[i].valid && slot[i].cnt != 0 && !busy)
                        slot[i].cnt <= slot[i].cnt - 1;

                // ---------- 发 draw_req ---------------
                draw_req <= (!busy && arb_found);

                if (!busy && arb_found) begin
                    col_base <= slot[arb_slot].col;
                    flip     <= slot[arb_slot].flip;
                    frame_id <= slot[arb_slot].frame;
                    row_off  <= slot[arb_slot].rowoff;

                    clear_slot <= arb_slot;     // <— 新增寄存器 (声明 logic [PTRW-1:0] clear_slot;)
                    clear_pending <= 1'b1;      // <— 新增寄存器 (logic clear_pending;)
                end

                // -------- 在下一拍清除 valid ----------
                if (clear_pending) begin
                    slot[clear_slot].valid <= 1'b0;
                    clear_pending          <= 1'b0;
                end


                // ---------- 行完成判断 ---------------
                any_v <= tmp_any;
                if (!busy && scan_complete && !any_v)
                    fe_done <= 1'b1;


            end

            
        end
    end
endmodule
