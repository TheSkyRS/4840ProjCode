import os
import csv
from PIL import Image
import tkinter as tk
from tkinter import filedialog

def rgba_to_rgb555t(pixel):
    r, g, b, a = pixel
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    t = 0  # Forced opacity
    return f"{(t << 15) | (r5 << 10) | (g5 << 5) | b5:04X}"

def image_to_tile_lines(image_path):
    img = Image.open(image_path).convert('RGBA')
    assert img.size == (16, 16)
    lines = []
    for y in range(16):
        line = [rgba_to_rgb555t(img.getpixel((x, y))) for x in range(16)]
        lines.append(line)
    return lines

def save_tiles_csv(image_paths, output_path):
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        for img_path in image_paths:
            tile_lines = image_to_tile_lines(img_path)
            writer.writerows(tile_lines)
    print(f"✅ tiles.csv has been saved: {output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    img_paths = filedialog.askopenfilenames(title="choose all tile ", filetypes=[("Images", "*.png")])
    if img_paths:
        out_path = filedialog.asksaveasfilename(title="save tiles.csv", defaultextension=".csv")
        if out_path:
            save_tiles_csv(sorted(img_paths), out_path)
