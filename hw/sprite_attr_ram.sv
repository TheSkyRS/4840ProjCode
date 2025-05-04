module sprite_attr_ram #(
    parameter NUM_SPRITE = 32
)(
    input logic clk,
    input logic [$clog2(NUM_SPRITE)-1:0] ra,
    input logic [$clog2(NUM_SPRITE)-1:0] wa,
    input logic write,
    input logic [31:0] d,
    output logic [31:0] q);
    logic [31:0] mem [NUM_SPRITE];
    always_ff @(posedge clk) begin
    if (write) mem[wa] <= d;
        q <= mem[ra];
    end
endmodule


/*
80 to ff address (byte address)
1000 0000 to 1111 1111
0010 0000 to 0011 1111 (word address)
*/