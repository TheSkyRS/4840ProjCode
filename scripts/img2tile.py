from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog
import csv

TILE_SIZE = 16

def get_tile_data(image, x, y):
    """提取指定位置tile的像素数据"""
    tile = []
    for dy in range(TILE_SIZE):
        for dx in range(TILE_SIZE):
            pixel = image.getpixel((x + dx, y + dy))
            tile.append(pixel)
    return tile

def save_tile_data(tiles, output_path):
    """保存tile集，每tile分成16行，每行16个像素，每个像素16位（RGB555+透明）"""
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        for tile in tiles:
            for row_idx in range(16):
                row = []
                for col_idx in range(16):
                    pixel = tile[row_idx * 16 + col_idx]
                    r, g, b = pixel[:3]
                    a = pixel[3] if len(pixel) == 4 else 255
                    r5 = (r >> 3) & 0x1F
                    g5 = (g >> 3) & 0x1F
                    b5 = (b >> 3) & 0x1F
                    transparency_bit = 1 if a < 128 else 0
                    rgb555t = (transparency_bit << 15) | (r5 << 10) | (g5 << 5) | b5
                    row.append(f"{rgb555t:04x}")
                writer.writerow(row)

def save_tilemap(tilemap, output_path):
    """保存tilemap"""
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(tilemap)

def process_image(image_path):
    image = Image.open(image_path).convert('RGB')
    width, height = image.size

    if width != 640 or height != 480:
        raise ValueError("图片大小不是640x480！")

    tiles = []
    tilemap = []

    for y in range(0, height, TILE_SIZE):
        row = []
        for x in range(0, width, TILE_SIZE):
            tile_data = get_tile_data(image, x, y)

            # 查找是否已有相同的tile
            matched = False
            for idx, existing_tile in enumerate(tiles):
                if tile_data == existing_tile:
                    row.append(idx)
                    matched = True
                    break

            if not matched:
                tiles.append(tile_data)
                row.append(len(tiles) - 1)

        tilemap.append(row)

    dir_name = os.path.dirname(image_path)

    tile_output_path = os.path.join(dir_name, "tiles.csv")
    tilemap_output_path = os.path.join(dir_name, "tilemap.csv")

    save_tile_data(tiles, tile_output_path)
    save_tilemap(tilemap, tilemap_output_path)

    print(f"处理完成！总不同tile数: {len(tiles)}")
    print(f"tile数据保存到: {tile_output_path}")
    print(f"tilemap保存到: {tilemap_output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    print("请选择要处理的图片 (640x480, RGB格式)")
    image_path = filedialog.askopenfilename(
        title="选择图片",
        filetypes=[("Image files", "*.png *.bmp *.jpg *.jpeg")]
    )

    if image_path:
        process_image(image_path)
    else:
        print("未选择文件。")