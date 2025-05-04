
from PIL import Image
import tkinter as tk
from tkinter import filedialog
import os

def overlay_images(background_path, overlay_path):
    # 打开两张图片
    background = Image.open(background_path).convert('RGBA')
    overlay = Image.open(overlay_path).convert('RGBA')

    # 计算将overlay放到background中间的位置
    bg_width, bg_height = background.size
    ol_width, ol_height = overlay.size
    position = ((bg_width - ol_width) // 2, (bg_height - ol_height) // 2)

    # 将overlay贴到background上
    result = background.copy()
    result.paste(overlay, position, overlay)  # 第三个参数是mask，支持透明通道

    # 保存新图
    dir_name = os.path.dirname(background_path)
    new_path = os.path.join(dir_name, "merged_image.png")
    result.save(new_path)

    return new_path

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    # 选择背景图片
    print("请选择背景图片")
    background_path = filedialog.askopenfilename(
        title="选择背景图片",
        filetypes=[("Image files", "*.jpg *.jpeg *.png *.bmp *.gif")]
    )

    if not background_path:
        print("未选择背景图片，程序结束。")
        exit()

    # 选择叠加图片
    print("请选择叠加图片")
    overlay_path = filedialog.askopenfilename(
        title="选择叠加图片",
        filetypes=[("Image files", "*.jpg *.jpeg *.png *.bmp *.gif")]
    )

    if not overlay_path:
        print("未选择叠加图片，程序结束。")
        exit()

    # 融合处理
    merged_path = overlay_images(background_path, overlay_path)
    print(f"处理完成！保存到: {merged_path}")