from PIL import Image
import csv
import os
import tkinter as tk
from tkinter import filedialog

TILE_SIZE = 16
MAP_WIDTH = 40
MAP_HEIGHT = 30

def load_tiles(tiles_csv):
    with open(tiles_csv, 'r') as f:
        reader = csv.reader(f)
        rows = list(reader)

    tiles = []
    for i in range(0, len(rows), TILE_SIZE):
        tile = rows[i:i + TILE_SIZE]
        tiles.append(tile)
    return tiles

def hex_to_rgb555(hexstr):
    val = int(hexstr, 16)
    r = ((val >> 10) & 0x1F) << 3
    g = ((val >> 5) & 0x1F) << 3
    b = (val & 0x1F) << 3
    t = (val >> 15) & 0x01
    return (r, g, b, 0 if t else 255)

def render_tilemap(tiles, tilemap_csv):
    with open(tilemap_csv, 'r') as f:
        reader = csv.reader(f)
        tilemap = [list(map(int, row)) for row in reader]

    img = Image.new('RGBA', (MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE), (0, 0, 0, 0))
    for y, row in enumerate(tilemap):
        for x, tile_id in enumerate(row):
            if 0 <= tile_id < len(tiles):
                tile = tiles[tile_id]
            else:
                tile = [['0000'] * TILE_SIZE for _ in range(TILE_SIZE)]
            for ty in range(TILE_SIZE):
                for tx in range(TILE_SIZE):
                    img.putpixel((x * TILE_SIZE + tx, y * TILE_SIZE + ty), hex_to_rgb555(tile[ty][tx]))
    return img

def main():
    root = tk.Tk()
    root.withdraw()

    tiles_path = filedialog.askopenfilename(title="choose tiles.csv document")
    tilemap_path = filedialog.askopenfilename(title="choose tilemap.csv document")
    if not tiles_path or not tilemap_path:
        print("❌ not choose document")
        return

    tiles = load_tiles(tiles_path)
    output_image = render_tilemap(tiles, tilemap_path)

    out_path = filedialog.asksaveasfilename(
        defaultextension=".png",
        filetypes=[("PNG Image", "*.png")],
        title="save the recover figure",
        initialfile="reconstructed_background.png"
    )
    if out_path:
        output_image.save(out_path)
        print(f"✅ figure has benn saved: {out_path}")

if __name__ == "__main__":
    main()
