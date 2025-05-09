#!/usr/bin/env bash
# make_audio_rom.sh  ──  MP3 → 8 kHz/8-bit PCM → audio_rom.hex  +  Verilog table
set -euo pipefail

OUT_HEX="audio_rom.hex"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

addr=0          # 累加 byte 地址
begins=()       # clip 起始地址表
ends=()         # clip 结束地址表

> "$OUT_HEX"    # 清空输出 HEX

for mp3 in "$@"; do
    base="$(basename "$mp3")"
    raw="$TMP_DIR/${base%.mp3}.raw"

    # 1) 转码：8 kHz、单声道、8-bit unsigned
    sox "$mp3" -r 8000 -c 1 -b 8 -e unsigned-integer "$raw"

    # 2) 文件大小（Linux / macOS 通用）
    bytes=$(wc -c < "$raw" | tr -d '[:space:]')
    printf "Packed %-20s (%5d bytes) at 0x%05x\n" "$base" "$bytes" "$addr"

    # 3) 追加 HEX（每行 1 byte → Quartus BRAM 初始化最稳）
    xxd -p -c1 "$raw" >> "$OUT_HEX"

    begins+=("18'h$(printf '%x' "$addr")")
    ends+=("18'h$(printf '%x' $((addr + bytes - 1)) )")

    addr=$((addr + bytes))
done

echo
echo "=== Verilog table ==="
printf "logic [17:0] sound_begin_addresses [] = '{ %s };\n" "$(IFS=,; echo "${begins[*]}")"
printf "logic [17:0] sound_end_addresses   [] = '{ %s };\n" "$(IFS=,; echo "${ends[*]}")"
echo
echo "Total ROM size: $addr bytes  (≤ 163840 OK? )"
