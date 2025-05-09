/* --------------------------------------------------------------------
 * Simple 8-bit synchronous ROM for audio samples
 * Depth = 0x24000 (≈150 k)  ⇒ fits in < 160 kB BRAM
 * Populate it with   $ readmemh("audio_rom.hex", mem)
 * ------------------------------------------------------------------*/
module audio_rom
#(parameter DEPTH = 18'h24000)
(
    input  logic [17:0] addr,  // 18-bit address
    input  logic        clk,
    output logic [7:0]  q
);
    logic [7:0] mem [0:DEPTH-1];

    initial $readmemh("audio_rom.hex", mem);  // hex/ASCII file

    always_ff @(posedge clk)
        q <= mem[addr];
endmodule
