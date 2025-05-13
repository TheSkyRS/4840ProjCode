from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog

def rgb888_to_rgb555_no_alpha(color):
    r, g, b, a = color
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    # RGB555 -> RGB888 
    return (r5 << 3, g5 << 3, b5 << 3, 255)

def process_image_to_16x16_rgb555_white_bg(image_path):
    image = Image.open(image_path).convert('RGBA')
    original_width, original_height = image.size

    # white no transparent
    target = Image.new('RGBA', (16, 16), (255, 255, 255, 255))

    # mid
    ratio = min(16 / original_width, 16 / original_height)
    new_size = (int(original_width * ratio), int(original_height * ratio))
    resized = image.resize(new_size, Image.LANCZOS)
    paste_x = (16 - new_size[0]) // 2
    paste_y = (16 - new_size[1]) // 2
    target.paste(resized, (paste_x, paste_y), mask=resized)

    # turn to RGB555 
    pixels = [rgb888_to_rgb555_no_alpha(p) for p in target.getdata()]
    result_image = Image.new('RGBA', (16, 16))
    result_image.putdata(pixels)

    return result_image

def batch_convert_to_rgb555_16x16_white(input_paths, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    for i, path in enumerate(sorted(input_paths)):
        img = process_image_to_16x16_rgb555_white_bg(path)
        new_name = f"tile_rgb555_{i:03d}.png"
        out_path = os.path.join(output_dir, new_name)
        img.save(out_path)
        print(f"✅ save: {new_name}")
    print(f"\n🎉 save to : {output_dir}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    img_paths = filedialog.askopenfilenames(
        title="choose the figure",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")]
    )

    if img_paths:
        out_dir = filedialog.askdirectory(title="choose the category")
        if out_dir:
            batch_convert_to_rgb555_16x16_white(img_paths, out_dir)
        else:
            print("⚠️ no chose category")
    else:
        print("⚠️ no chose figure")
