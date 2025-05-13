from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog

def rgb888_to_rgb555_no_alpha(color):
    r, g, b, a = color
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    return (r5 << 3, g5 << 3, b5 << 3, 255)  # Forced opacity

def process_image_rgb555_resized(image_path):
    image = Image.open(image_path).convert('RGBA')
    resized = image.resize((16, 16), Image.LANCZOS)

    pixels = [rgb888_to_rgb555_no_alpha(p) for p in resized.getdata()]
    result_image = Image.new('RGBA', (16, 16))
    result_image.putdata(pixels)

    return result_image

def batch_convert_fixed_resize_rgb555():
    root = tk.Tk()
    root.withdraw()

    input_paths = filedialog.askopenfilenames(
        title="choose figure（all into 16×16 to RGB555）",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")]
    )
    if not input_paths:
        print("⚠️ no choose figure")
        return

    output_dir = filedialog.askdirectory(title="choose file")
    if not output_dir:
        print("⚠️ no choose path")
        return

    os.makedirs(output_dir, exist_ok=True)

    for i, path in enumerate(sorted(input_paths)):
        img = process_image_rgb555_resized(path)
        out_name = f"tile_rgb555_{i:03d}.png"
        img.save(os.path.join(output_dir, out_name))
        print(f"✅ save: {out_name}")

    print("\n🎉 All process has been processed！")

if __name__ == "__main__":
    batch_convert_fixed_resize_rgb555()
