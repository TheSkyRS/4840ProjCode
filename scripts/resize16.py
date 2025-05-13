from PIL import Image
import tkinter as tk
from tkinter import filedialog
import os

def resize_to_16x16(image_path, output_path):
    image = Image.open(image_path).convert('RGBA')
    original_width, original_height = image.size

    
    target_size = (16, 16)
    new_image = Image.new('RGBA', target_size, (0, 0, 0, 0))

   
    ratio = min(16 / original_width, 16 / original_height)
    new_size = (int(original_width * ratio), int(original_height * ratio))
    resized_image = image.resize(new_size, Image.LANCZOS)

    
    paste_x = (16 - new_size[0]) // 2
    paste_y = (16 - new_size[1]) // 2
    new_image.paste(resized_image, (paste_x, paste_y), mask=resized_image)

    new_image.save(output_path)
    print(f"✅ It has been saved as a 16×16 image: {output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    input_path = filedialog.askopenfilename(
        title="save figures",
        filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")]
    )

    if input_path:
        dir_name = os.path.dirname(input_path)
        base_name = os.path.basename(input_path)
        name, ext = os.path.splitext(base_name)
        output_path = os.path.join(dir_name, f"{name}_16x16.png")
        resize_to_16x16(input_path, output_path)
    else:
        print("❌ no choose figures")
