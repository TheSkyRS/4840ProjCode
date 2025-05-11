module ffi_audio #(
    parameter ADDR_WIDTH = 14   
)(
    input  logic          clk50,    
    input  logic          reset,
    /* Avalon-MM slave */
    input  logic  [31:0]  writedata,
    input  logic          write,
    input  logic          chipselect,
    input  logic  [3:0]   address,   // 0:CONTROL
    /* Avalon-ST source -> audio_0 */
    input  logic          L_READY,
    input  logic          R_READY,
    output logic [15:0]   L_DATA,
    output logic [15:0]   R_DATA,
    output logic          L_VALID,
    output logic          R_VALID
);
    logic [1:0] clip_sel;      // CONTROL_REG[4:3] 传入
    logic [ADDR_WIDTH-1:0] rom_addr;
    logic playing;             // FIFO/计数器状态

    logic [12:0] div;          // 50e6 / 16_000 ≈ 3125
    always_ff @(posedge clk50) begin
        if (reset) begin
            div <= 13'd0; L_VALID<=0; R_VALID<=0; playing<=0;
        end else begin
            if (chipselect && write && address==4'd0) begin
                clip_sel <= writedata[4:3];
                playing  <= 1'b1;         // 边写边触发
                rom_addr <= '0;
            end

            if (playing) begin
                if (div==13'd0) begin
                    div <= 13'd3124;
                    rom_addr <= rom_addr + 1'b1;
                    L_VALID <= 1; R_VALID<=1;
                end else begin
                    div <= div - 1'b1;
                    L_VALID <= 0; R_VALID<=0;
                end
            end
            if (rom_addr == ROM_LEN-1) playing<=0; // clip 结束
        end
    end

    logic [15:0] rom_out0, rom_out1, rom_out2, rom_out3;
    always_comb begin
        unique case (clip_sel)
          2'd0: {L_DATA,R_DATA} = {rom_out0,rom_out0};
          2'd1: {L_DATA,R_DATA} = {rom_out1,rom_out1};
          2'd2: {L_DATA,R_DATA} = {rom_out2,rom_out2};
          2'd3: {L_DATA,R_DATA} = {rom_out3,rom_out3};
        endcase
    end
endmodule
