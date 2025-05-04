module sprite_frontend #(
    parameter int NUM_SPRITE = 32,
    parameter int MAX_SLOT   = 8                 // 2 的整数次幂
)(
    input  logic           clk,
    input  logic           reset,

    input  logic           start_row,
    input  logic [9:0]     next_vcount,

    output logic [$clog2(NUM_SPRITE)-1:0] ra,
    input  logic [31:0]    rd_data,

    input  logic           draw_done,           // drawer: 1‑idle 0‑busy
    output logic           draw_req,
    output logic [9:0]     col_base,
    output logic           flip,
    output logic [7:0]     frame_id,
    output logic [3:0]     row_off,

    output logic           fe_done
);

    localparam int IDXW  = $clog2(NUM_SPRITE);
    localparam int QPW   = $clog2(MAX_SLOT);
    localparam int MASK  = MAX_SLOT - 1;

    // 扫描索引 0‥NUM_SPRITE
    logic [IDXW:0] scan_idx;
    assign ra = scan_idx[IDXW-1:0];

    // 环形 FIFO
    typedef struct packed { logic [9:0] col; logic flip; logic [7:0] frame; logic [3:0] rowoff; } ent_t;
    ent_t           fifo [MAX_SLOT];
    logic [QPW-1:0] head, tail;
    logic [QPW:0]   cnt;                 // 0‥MAX_SLOT

    always_ff @(posedge clk) begin
        //------------------------------------------------ reset / blank
        if (reset) begin
            scan_idx <= NUM_SPRITE;
            head<=0; tail<=0; cnt<=0;
            draw_req<=0; fe_done<=1;
        end
        else if (start_row) begin
            head<=0; tail<=0; cnt<=0; draw_req<=0;
            if (next_vcount < 10'd480) begin
                scan_idx <= 0;  fe_done <= 0;      // 可见行
            end else begin
                scan_idx <= NUM_SPRITE; fe_done <= 1; // blank
            end
        end
        //----------------------------- normal run
        else begin
            draw_req<=0; fe_done<=0;

            // 扫描属性
            if (scan_idx < NUM_SPRITE) begin
                logic en = rd_data[31];
                logic [8:0] top = rd_data[26:18];
                logic hit = en && next_vcount >= top && next_vcount < top+16;
                if (hit && cnt < MAX_SLOT) begin   // 入队（队列满则丢弃）
                    fifo[tail] <= '{ rd_data[17:8], rd_data[30],
                                     rd_data[7:0], (next_vcount-top)[3:0] };
                    tail <= (tail + 1'b1) & MASK;
                    cnt  <= cnt + 1'b1;
                end
                scan_idx <= scan_idx + 1'b1;
            end

            // drawer 空闲 且 队列非空 → 出队
            if (draw_done && cnt != 0) begin
                col_base <= fifo[head].col;
                flip     <= fifo[head].flip;
                frame_id <= fifo[head].frame;
                row_off  <= fifo[head].rowoff;
                draw_req <= 1;
                head <= (head + 1'b1) & MASK;
                cnt  <= cnt - 1'b1;
            end

            // 行完成
            if (scan_idx >= NUM_SPRITE && cnt==0 && draw_done)
                fe_done <= 1;
        end
    end
endmodule
