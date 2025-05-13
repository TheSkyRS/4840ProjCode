from PIL import Image
import csv
import os
import tkinter as tk
from tkinter import filedialog

TILE_SIZE = 16
TILE_WIDTH = 40
TILE_HEIGHT = 30

# 125~137 to 0~12
TILE_ID_MAP = {tile_id: tile_id - 125 for tile_id in range(125, 138)}

def load_tiles(tiles_csv_path):
    tiles = []
    with open(tiles_csv_path, newline='') as f:
        reader = csv.reader(f)
        lines = list(reader)
        assert len(lines) % TILE_SIZE == 0, "The number of lines in Ties.csv must be a multiple of 16"
        for i in range(0, len(lines), TILE_SIZE):
            tile = lines[i:i+TILE_SIZE]
            tiles.append(tile)
    return tiles

def load_tilemap(tilemap_csv_path):
    with open(tilemap_csv_path, newline='') as f:
        return [list(map(int, row)) for row in csv.reader(f)]

def hex_to_rgb555(val):
    num = int(val, 16)
    r = ((num >> 10) & 0x1F) << 3
    g = ((num >> 5) & 0x1F) << 3
    b = (num & 0x1F) << 3
    t = (num >> 15) & 0x1
    return (r, g, b, 0 if t else 255)

def reconstruct_image(tiles, tilemap):
    img = Image.new('RGBA', (TILE_WIDTH * TILE_SIZE, TILE_HEIGHT * TILE_SIZE), (0, 0, 0, 0))

    for y in range(TILE_HEIGHT):
        for x in range(TILE_WIDTH):
            tile_id_raw = tilemap[y][x]
            tile_id = TILE_ID_MAP.get(tile_id_raw, -1)  # failed return -1

            if 0 <= tile_id < len(tiles):
                tile = tiles[tile_id]
            else:
                tile = [["0000"] * TILE_SIZE for _ in range(TILE_SIZE)]

            for ty in range(TILE_SIZE):
                for tx in range(TILE_SIZE):
                    pixel = hex_to_rgb555(tile[ty][tx])
                    img.putpixel((x * TILE_SIZE + tx, y * TILE_SIZE + ty), pixel)

    return img

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    tiles_path = filedialog.askopenfilename(title="选择 tile.csv", filetypes=[("CSV", "*.csv")])
    if not tiles_path:
        print("❌ not choose tile.csv")
        exit()

    tilemap_path = filedialog.askopenfilename(title="选择 tilemap.csv", filetypes=[("CSV", "*.csv")])
    if not tilemap_path:
        print("❌ not choose tilemap.csv")
        exit()

    tiles = load_tiles(tiles_path)
    tilemap = load_tilemap(tilemap_path)
    image = reconstruct_image(tiles, tilemap)

    save_path = filedialog.asksaveasfilename(
        title="save the recorey figures",
        defaultextension=".png",
        filetypes=[("PNG Image", "*.png")],
        initialfile="tilemap_reconstructed.png"
    )
    if save_path:
        image.save(save_path)
        print(f"✅ save the figures as: {save_path}")
    else:
        print("⚠️ not save figures")
