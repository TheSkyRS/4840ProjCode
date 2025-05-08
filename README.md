## VGA_TOP Register Map (Byte Addressed, 32-bit Wide)

| Offset | Register               | Description                                 | Valid Bits | Value Range           | R/W |
|:------:|:----------------------:|:-------------------------------------------:|:----------:|:---------------------:|:---:|
| 0x00   | CTRL_REG               | Control register (e.g. tilemap index)       | [1:0]      | 0–3                   |  W  |
| 0x04   | STATUS_REG             | Current pixel column and row                | [19:0]     | [19:10]col:0–639, [9:0]row:0–479  |  R  |
| 0x08–0x7F | Reserved            | Reserved for future use                     | —          | —                     | —   |
| 0x80–0xFF | SPRITE_ATTR_TABLE[n] | Sprite attribute table (32 entries)        | [31:0]     | See format below      | W |

---

### SPRITE_ATTR_TABLE Format (Each Entry = 4 Bytes)

Each entry at offset: `0x80 + (n * 4)`, where `n ∈ [0, 31]`

| Bits    | Field       | Description                          |
|---------|-------------|--------------------------------------|
| [31]    | `enable`    | 1 = visible, 0 = hidden               |
| [30]    | `flip`      | 1 = horizontally flipped              |
| [29:27] | Reserved    | Unused                               |
| [26:18] | `sprite_y`  | Vertical position (0–479)            |
| [17:8]  | `sprite_x`  | Horizontal position (0–639)          |
| [7:0]   | `frame_id`  | Sprite frame index (0–255)           |

---

### Notes

- All addresses are byte-aligned and 32-bit (4-byte) wide.
- Valid `sprite_attr_table[n]` range: `n = 0 to 31` → offset `0x80` to `0xFC`
- Only `0x00`, `0x04`, and `0x80–0xFF` are valid; others are reserved.