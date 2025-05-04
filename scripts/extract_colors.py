from PIL import Image
import tkinter as tk
from tkinter import filedialog

def quantize_color(color, threshold=4):
    """把颜色按阈值量化，减少比较次数"""
    factor = threshold + 1
    return tuple(c // factor for c in color)

def extract_unique_colors(image_path, threshold=4):
    image = Image.open(image_path).convert('RGB')
    pixels = list(image.getdata())

    quantized_colors = set()

    for pixel in pixels:
        q_color = quantize_color(pixel, threshold)
        quantized_colors.add(q_color)

    return quantized_colors, len(quantized_colors)

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()  # 隐藏主窗口

    image_path = filedialog.askopenfilename(
        title="选择一张图片",
        filetypes=[("Image files", "*.jpg *.jpeg *.png *.bmp *.gif")]
    )

    if image_path:
        colors, num_colors = extract_unique_colors(image_path, threshold=4)
        print(f"不同颜色数量: {num_colors}")
        # 如果你想查看所有不同的颜色，可以取消下面这一行的注释
        # print(colors)
    else:
        print("未选择文件。")