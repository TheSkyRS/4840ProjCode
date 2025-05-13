from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog

def resize_to_16x16(image_path):
    image = Image.open(image_path).convert('RGBA')
    original_width, original_height = image.size

    target_size = (16, 16)
    new_image = Image.new('RGBA', target_size, (0, 0, 0, 0))

    ratio = min(16 / original_width, 16 / original_height)
    new_size = (int(original_width * ratio), int(original_height * ratio))
    resized = image.resize(new_size, Image.LANCZOS)

    paste_x = (16 - new_size[0]) // 2
    paste_y = (16 - new_size[1]) // 2
    new_image.paste(resized, (paste_x, paste_y), mask=resized)

    return new_image

def batch_resize_and_rename(input_files, output_dir):
    os.makedirs(output_dir, exist_ok=True)
    for i, path in enumerate(sorted(input_files)):
        image = resize_to_16x16(path)
        new_name = f"sprite_{i:03d}.png"
        out_path = os.path.join(output_dir, new_name)
        image.save(out_path)
        print(f"✅ has been saved：{new_name}")
    print(f"\n🎉 all figures have been saved to : {output_dir}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    image_paths = filedialog.askopenfilenames(title="Select the image to be scaled", filetypes=[("Images", "*.png *.jpg *.jpeg")])
    if image_paths:
        out_dir = filedialog.askdirectory(title="Select the output folder")
        if out_dir:
            batch_resize_and_rename(image_paths, out_dir)
        else:
            print("⚠️ The output folder was not selected")
    else:
        print("⚠️ No picture selected")
