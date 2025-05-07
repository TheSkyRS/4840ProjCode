module sprite_frontend #(
    parameter NUM_SPRITE = 32,
    parameter MAX_SLOT   = 8
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

    logic [IDXW-1:0] scan_idx;
    logic [IDXW-1:0] scan_idx_d;
    logic [IDXW:0] scan_idx_checked; // NUM_SPRITE
    assign ra = scan_idx;

    logic hit;
    assign hit = rd_data[31] && (next_vcount >= {1'b0,rd_data[26:18]}) && (next_vcount < rd_data[26:18]+16);
    // 环形 FIFO
    typedef struct packed { logic [9:0] col; logic flip; logic [7:0] frame; logic [3:0] rowoff; } ent_t;
    ent_t           fifo [MAX_SLOT];
    logic [QPW-1:0] head, tail;
    logic [QPW-1:0]   cnt;
    assign cnt = (tail-head) & MASK;

    logic drawing;

    always_ff @(posedge clk) begin
        //------------------------------------------------ reset / blank
        if (reset) begin
            scan_idx <= 0;
            scan_idx_d <= 0;
            scan_idx_checked <= NUM_SPRITE;
            head<=0;
            tail<=0;
            draw_req<=0;
            drawing <= 0;
            fe_done<=1;
        end
        else if (start_row) begin
            scan_idx <= 0;
            scan_idx_d <= 0;
            scan_idx_checked <= NUM_SPRITE;
            head<=0;
            tail<=0;
            draw_req<=0;
            drawing <= 0;
            if (next_vcount < 10'd480) begin
                fe_done <= 0;      // 可见行
            end else begin
                fe_done <= 1; // blank
            end
        end
        //----------------------------- normal run
        else if (!fe_done) begin
            draw_req <= 0;

            scan_idx_d <= scan_idx;
            /*
            Why we need "-2": Important, because if we just -1, scan_idx will become 9 and keep this value, scan_idx_d become 8 for 1 clk,
            then after 1clk, scan_idx_d become 9, so we lost 8(scan_idx_d)'s value, it becomes 9's value, waiting for enqueue.
            */
            if(scan_idx < NUM_SPRITE - 1 && cnt < MAX_SLOT - 2) begin
                scan_idx <= scan_idx + 1'b1;
            end
            // Enqueue
            if (hit && cnt < MAX_SLOT - 1 && ({1'b0,scan_idx_d} != scan_idx_checked) && scan_idx > 0) begin
                scan_idx_checked <= {1'b0,scan_idx_d};
                fifo[tail].col <= rd_data[17:8];
                fifo[tail].flip <= rd_data[30];
                fifo[tail].frame <=rd_data[7:0];
                fifo[tail].rowoff <= (next_vcount-rd_data[26:18]) & 4'hf;
                tail <= (tail + 1'b1) & MASK;
            end

            // drawer 空闲 且 队列非空 → Dequeue
            if (!drawing && cnt != 0) begin
                col_base <= fifo[head].col;
                flip     <= fifo[head].flip;
                frame_id <= fifo[head].frame;
                row_off  <= fifo[head].rowoff;
                // 1 clk pulse
                draw_req <= 1;

                // drawing state long period
                drawing <= 1;
                head <= (head + 1'b1) & MASK;
            end

            if(drawing) begin
                // draw_req <= 0;
                // Warning!!: should be very careful, because "after" draw_req 1 clk pulse, draw_done set to 0. 
                // So we can not quickly check draw_done, since it is "1" when draw_req = 1
                // After draw_req drop to 0(lasting 1 clk), draw_done set to 0. Then we can check draw_done to detect "drawing" state.
                if (draw_done && !draw_req)
                    drawing <= 0;
            end

            // 行完成
            if (scan_idx_d == NUM_SPRITE-1 && cnt==0 && draw_done)
                fe_done <= 1;
        end
    end
endmodule
