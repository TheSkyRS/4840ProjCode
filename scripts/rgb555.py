from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog

def rgb888_to_rgb555(color):
    r, g, b, a = color
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    print(a)
    transparency_bit = 1 if a < 128 else 0
    return (r5 << 3, g5 << 3, b5 << 3, 0 if transparency_bit else 255)

def convert_image_to_rgb555(image_path):
    image = Image.open(image_path).convert('RGBA')

    pixels = list(image.getdata())

    # 将每个像素都进行RGB555转换
    new_pixels = [rgb888_to_rgb555(pixel) for pixel in pixels]

    # 创建新图片
    new_image = Image.new('RGBA', image.size)
    new_image.putdata(new_pixels)

    # 保存到原目录，文件名加后缀
    dir_name = os.path.dirname(image_path)
    base_name = os.path.basename(image_path)
    name, ext = os.path.splitext(base_name)
    new_path = os.path.join(dir_name, f"{name}_RGB555.png")
    new_image.save(new_path)

    return new_path

if __name__ == "__main__":
    
    root = tk.Tk()
    root.withdraw()

    image_path = filedialog.askopenfilename(
        title="选择一张图片",
        filetypes=[("Image files", "*.jpg *.jpeg *.png *.bmp *.gif")]
    )

    if image_path:
        new_image_path = convert_image_to_rgb555(image_path)
        print(f"处理完成！保存到: {new_image_path}")
    else:
        print("未选择文件。")