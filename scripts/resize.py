from PIL import Image
import tkinter as tk
from tkinter import filedialog
import os

def resize_image_keep_aspect_ratio(image_path, output_path, target_size=(640, 480)):
    image = Image.open(image_path).convert('RGBA')
    original_width, original_height = image.size
    target_width, target_height = target_size

    # 创建透明背景
    new_image = Image.new('RGBA', (target_width, target_height), (0, 0, 0, 0))

    if original_width <= target_width and original_height <= target_height:
        # 图片较小，直接粘贴到左上角
        new_image.paste(image, (0, 0))
    else:
        # 等比缩放并居中
        ratio = min(target_width / original_width, target_height / original_height)
        new_size = (int(original_width * ratio), int(original_height * ratio))
        resized_image = image.resize(new_size, Image.LANCZOS)

        paste_x = (target_width - new_size[0]) // 2
        paste_y = (target_height - new_size[1]) // 2
        new_image.paste(resized_image, (paste_x, paste_y))

    new_image.save(output_path)
    print(f"保存成功：{output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    input_path = filedialog.askopenfilename(
        title="选择图片",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")]
    )

    if input_path:
        dir_name = os.path.dirname(input_path)
        base_name = os.path.basename(input_path)
        name, ext = os.path.splitext(base_name)
        output_path = os.path.join(dir_name, f"{name}_resized.png")
        resize_image_keep_aspect_ratio(input_path, output_path)
    else:
        print("未选择图片")