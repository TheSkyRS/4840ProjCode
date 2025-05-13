from PIL import Image
import csv
import os
import tkinter as tk
from tkinter import filedialog

def rgba_to_rgb555t(rgba):
    r, g, b, a = rgba
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    t = 0 if a >= 128 else 1
    return f"{(t << 15) | (r5 << 10) | (g5 << 5) | b5:04x}"

def sprite_to_csv(image_path, output_path):
    img = Image.open(image_path).convert('RGBA')
    if img.size != (16, 16):
        raise ValueError("The image is not 16×16 in size")
    
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        for y in range(16):
            row = [rgba_to_rgb555t(img.getpixel((x, y))) for x in range(16)]
            writer.writerow(row)
    print(f"✅ A CSV file has been generated: {output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    img_path = filedialog.askopenfilename(filetypes=[("PNG", "*.png")])
    if img_path:
        out_path = img_path.replace(".png", ".csv")
        sprite_to_csv(img_path, out_path)
