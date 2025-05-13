import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image
import os

def rgb555_to_rgb888(value):
    """Convert a 16-bit RGB555 value to an RGB888 tuple."""
    r = (value >> 10) & 0x1F
    g = (value >> 5) & 0x1F
    b = value & 0x1F
    return (r << 3, g << 3, b << 3)

def extract_sprites(mif_path, output_folder):
    try:
        with open(mif_path, "r") as file:
            lines = file.readlines()

        # 过滤出含地址的数据行
        data_lines = [line for line in lines if ":" in line and ";" in line]

        # 转换为 RGB888 像素列表
        pixels = []
        for line in data_lines:
            _, value = line.strip().split(":")
            value = value.strip(" ;")
            value_int = int(value, 16)
            pixels.append(rgb555_to_rgb888(value_int))

        # 每张图片是 256 像素（16x16），共 65 张
        num_sprites = len(pixels) // 256
        for i in range(num_sprites):
            sprite_pixels = pixels[i*256:(i+1)*256]
            img = Image.new("RGB", (16, 16))
            img.putdata(sprite_pixels)
            save_path = os.path.join(output_folder, f"sprite_{i:02d}.png")
            img.save(save_path)

        messagebox.showinfo("成功", f"已生成 {num_sprites} 张 sprite 图像，保存在：\n{output_folder}")
    except Exception as e:
        messagebox.showerror("错误", f"处理失败：\n{str(e)}")

def run_gui():
    root = tk.Tk()
    root.withdraw()

    mif_path = filedialog.askopenfilename(
        title="选择 MIF 文件",
        filetypes=[("MIF 文件", "*.mif"), ("所有文件", "*.*")]
    )
    if not mif_path:
        messagebox.showinfo("取消", "未选择输入文件。")
        return

    output_folder = filedialog.askdirectory(
        title="选择输出文件夹"
    )
    if not output_folder:
        messagebox.showinfo("取消", "未选择输出目录。")
        return

    extract_sprites(mif_path, output_folder)

if __name__ == "__main__":
    run_gui()
