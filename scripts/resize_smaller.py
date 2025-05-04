from PIL import Image
import os

def expand_and_resize_image(image_path, output_path):
    # 打开图片，并确保是RGBA格式（带透明通道）
    image = Image.open(image_path).convert('RGBA')

    # 原始大小
    original_width, original_height = image.size
    assert (original_width, original_height) == (640, 480), "输入图片必须是640x480！"

    # 目标扩展尺寸
    expanded_width, expanded_height = 960, 720

    # 创建新的透明背景图
    expanded_image = Image.new('RGBA', (expanded_width, expanded_height), (0, 0, 0, 0))

    # 将原图粘贴到中间
    paste_x = (expanded_width - original_width) // 2
    paste_y = (expanded_height - original_height) // 2
    expanded_image.paste(image, (paste_x, paste_y))

    # 再缩放回640x480，使用高质量缩放
    resized_image = expanded_image.resize((640, 480), Image.LANCZOS)

    # 保存
    resized_image.save(output_path)

    print(f"处理完成！保存到: {output_path}")

if __name__ == "__main__":
    import tkinter as tk
    from tkinter import filedialog

    root = tk.Tk()
    root.withdraw()

    print("请选择输入图片 (640x480)")
    input_path = filedialog.askopenfilename(
        title="选择图片",
        filetypes=[("Image files", "*.png *.bmp *.jpg *.jpeg")]
    )

    if input_path:
        dir_name = os.path.dirname(input_path)
        base_name = os.path.basename(input_path)
        name, ext = os.path.splitext(base_name)

        output_path = os.path.join(dir_name, f"{name}_smaller.png")
        expand_and_resize_image(input_path, output_path)
    else:
        print("未选择文件。")