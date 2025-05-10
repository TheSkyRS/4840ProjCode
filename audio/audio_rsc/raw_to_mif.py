#!/usr/bin/env python3
import pathlib, struct, sys

raw = 'jumpfb.raw'
mif = 'jumpfb.mif'
data = pathlib.Path(raw).read_bytes()
depth = len(data)

lines = [
    f'WIDTH=8;',
    f'DEPTH={depth};',
    'ADDRESS_RADIX=HEX;',
    'DATA_RADIX=HEX;',
    'CONTENT BEGIN'
]

for addr, byte in enumerate(data):
    lines.append(f'    {addr:04X} : {byte:02X};')

lines.append('END;')
pathlib.Path(mif).write_text('\n'.join(lines))
print(f'Wrote {mif} with {depth} bytes.')
