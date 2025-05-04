from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog
import csv

TILE_SIZE = 16

def rgb555_to_rgba888(value):
    """RGB555+透明转RGBA8888，透明时RGB置0"""
    transparency_bit = (value >> 15) & 0x1
    r = ((value >> 10) & 0x1F) << 3
    g = ((value >> 5) & 0x1F) << 3
    b = (value & 0x1F) << 3
    a = 0 if transparency_bit == 1 else 255
    if a == 0:
        return (0, 0, 0, 0)
    return (r, g, b, a)

def load_tiles_csv(tiles_csv_path):
    """读取tiles.csv，每16行组成一个tile的像素数据"""
    tiles = []
    with open(tiles_csv_path, 'r') as f:
        reader = csv.reader(f)
        tile_pixels = []
        for idx, row in enumerate(reader):
            for hex_str in row:
                value = int(hex_str, 16)
                tile_pixels.append(rgb555_to_rgba888(value))
            if (idx + 1) % 16 == 0:
                tiles.append(tile_pixels)
                tile_pixels = []
    return tiles

def load_tilemap_csv(tilemap_path):
    """读取tilemap.csv"""
    with open(tilemap_path, 'r') as f:
        reader = csv.reader(f)
        tilemap = [[int(cell) for cell in row] for row in reader]
    return tilemap

def reconstruct_image(tilemap, tiles):
    """根据tilemap和tiles拼接成一张大图"""
    rows = len(tilemap)
    cols = len(tilemap[0]) if rows > 0 else 0

    image = Image.new('RGBA', (cols * TILE_SIZE, rows * TILE_SIZE), (0, 0, 0, 0))

    for y, row in enumerate(tilemap):
        for x, tile_idx in enumerate(row):
            tile_pixels = tiles[tile_idx]
            tile_img = Image.new('RGBA', (TILE_SIZE, TILE_SIZE))
            tile_img.putdata(tile_pixels)
            image.paste(tile_img, (x * TILE_SIZE, y * TILE_SIZE), tile_img)
    
    return image

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    # 选择tiles.csv
    print("请选择tiles.csv")
    tiles_csv_path = filedialog.askopenfilename(
        title="选择tiles.csv",
        filetypes=[("CSV files", "*.csv")]
    )

    if not tiles_csv_path:
        print("未选择tiles文件。")
        exit()

    # 选择tilemap.csv
    print("请选择tilemap.csv")
    tilemap_path = filedialog.askopenfilename(
        title="选择tilemap.csv",
        filetypes=[("CSV files", "*.csv")]
    )

    if not tilemap_path:
        print("未选择tilemap文件。")
        exit()

    # 加载数据
    tiles = load_tiles_csv(tiles_csv_path)
    tilemap = load_tilemap_csv(tilemap_path)

    # 生成图片
    final_image = reconstruct_image(tilemap, tiles)

    # 保存
    save_dir = os.path.dirname(tiles_csv_path)
    save_path = os.path.join(save_dir, "reconstructed_image.png")
    final_image.save(save_path)

    print(f"重建完成！保存到: {save_path}")