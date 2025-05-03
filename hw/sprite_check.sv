// -----------------------------------------------------------------------------
//  sprite_check.sv
// -----------------------------------------------------------------------------
//  1. 在行开始 (start=1) 时扫描 NUM_SPRITE 个 sprite_attr，找出本行可见的前 MAX_ACTIVE 个。
//  2. 对每一个命中的 sprite：
//       • 计算 down‑counter 初值 (sprite_pos_col)
//       • 读取该行 16px 的像素 (256bit) 并考虑 Flip 位
//       • 保存 Priority
//  3. 全部完成后拉高 ready，sprite_engine 即可使用 slot_* 阵列。
// -----------------------------------------------------------------------------
module sprite_check #(
    parameter NUM_SPRITE = 32,
    parameter MAX_ACTIVE = 8

) (
    input logic clk,
    input logic reset,
    input logic start,
    input logic [9:0] next_vcount,

    input logic [31:0] sprite_attr_table[NUM_SPRITE],

    // output to sprite_engine
    output logic ready, //这一行的slots都填好了
    output logic [MAX_ACTIVE-1:0] slot_valid,
    output logic [MAX_ACTIVE-1:0][9:0] slot_counter_init, // =sprite_pos_col
    output logic [MAX_ACTIVE-1:0][255:0] slot_pixel_row, // 16px * 16bit
    output logic [MAX_ACTIVE-1:0][1:0] slot_priority);

// =============================================================================
//  内部：Sprite ROM  ‑‑ 16bit/px, 同步 1clk 延迟
// =============================================================================
logic [15:0] rom_addr;
logic [15:0] rom_q;

sprite_rom u_sprite_rom (
    .clk(clk),
    .addr(rom_addr), // 0 - (256*256-1) = frame_id*256 + row*16+col
    .q(rom_q)
);

// =============================================================================
//  状态机 / 缓冲区
// =============================================================================
typedef enum logic [1:0] {IDLE, SCAN, LOAD, DONE} state_t;

state_t cs,ns;
logic [4:0] sprite_idx; //正在检查的sprite序号
logic [4:0] slot_idx;   // 正在写入的slot序号
logic [4:0] px_cnt;     // 0-16 计 17拍
logic [15:0] base_addr;

logic flip_q;
logic [1:0] prio_q;
logic [9:0] col_q;
logic [255:0] row_buf;


// ---------- slot 寄存器 ----------
logic [MAX_ACTIVE-1:0] vld_r;
logic [MAX_ACTIVE-1:0][9:0] col_r;
logic [MAX_ACTIVE-1:0][255:0] pix_r;
logic [MAX_ACTIVE-1:0][1:0] prio_r;

// =============================================================================
//  Next‑state logic
// =============================================================================
always_comb begin
    ns = cs;
    unique case (cs) 
        IDLE: if (start) ns = SCAN;

        SCAN: begin
            //条件：已经扫描完了所有的sprite or slot满了
            if (sprite_idx == NUM_SPRITE || slot_idx == MAX_ACTIVE)
                ns = DONE;
            else begin
                logic hit;

                hit = sprite_attr_table[sprite_idx][31] && //enable
                        next_vcount >= sprite_attr_table[sprite_idx][26:18] &&
                        next_vcount < sprite_attr_table[sprite_idx][26:18] + 16;
                ns = hit ? LOAD : SCAN;
            end
        end

        LOAD: if (px_cnt == 16) ns = SCAN; // 17拍: addr0->数据16

        DONE: if (!start) ns = IDLE;
    endcase
end


// =============================================================================
//  主时序
// =============================================================================
always_ff @(posedge clk) begin
    if (reset) begin
        cs <= IDLE;
        ready <= 0;
        sprite_idx <= 0;
        slot_idx <= 0;
        vld_r <= 0;
        px_cnt <= 0;
    end else begin
        cs <= ns;

        unique case (cs)
        // ----------------------------- IDLE ------------------------------
        IDLE: begin
            ready <= 0;
            sprite_idx <= 0;
            slot_idx <= 0;
            vld_r <= '0;
        end

        // ----------------------------- SCAN ------------------------------
        SCAN: begin
            if (ns == LOAD) begin
                // 记录当前sprite基本信息
                flip_q <= sprite_attr_table[sprite_idx][30];
                prio_q <= sprite_attr_table[sprite_idx][28:27];
                col_q <= sprite_attr_table[sprite_idx][17:8];
                // frame_id*256 + row*16
                base_addr <= {sprite_attr_table[sprite_idx][7:0], 8'b0} + (next_vcount - sprite_attr_table[sprite_idx][26:18])[3:0] << 4;
                px_cnt <= 0;
            end else begin
                // 未命中，扫描下一个sprite
                sprite_idx <= sprite_idx + 1;
            end
        end

        // ----------------------------- LOAD ------------------------------
        LOAD: begin
            //取像素: addr寄存器先发
            rom_addr <= base_addr + px_cnt;

            // 拼row_buf: rom_q为上拍返回
            if (px_cnt != 0) begin
                if (!flip_q)
                    row_buf[((px_cnt-1)<<4) +:16] <= rom_q; // 0 .. 15 顺序
                else
                    row_buf[((16-px_cnt)<<4) +:16] <= rom_q; // 15 .. 0 反向
            end

            px_cnt <= px_cnt+1;

            // 第16 -> 拍结束，rom_q已拿到最后一个像素
            if (px_cnt == 5'b16) begin
                // 写slot
                pix_r [slot_idx] <= row_buf;
                col_r [slot_idx] <= col_q;
                prio_r[slot_idx] <= prio_q;
                vld_r [slot_idx] <= 1'b1;

                // 准备下一 sprite
                slot_idx <= slot_idx + 1;
                sprite_idx <= sprite_idx + 1;
            end
        end

        // ----------------------------- DONE ------------------------------
        DONE: ready <= 1;
        endcase

        end
end

// =============================================================================
//  输出映射
// =============================================================================
assign slot_valid        = vld_r;
assign slot_counter_init = col_r;
assign slot_pixel_row    = pix_r;
assign slot_priority     = prio_r;


endmodule