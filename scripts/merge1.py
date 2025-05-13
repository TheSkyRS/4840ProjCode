from PIL import Image
import tkinter as tk
from tkinter import filedialog
import csv
import os

def rgba_to_rgb555t(rgba):
    r, g, b, a = rgba
    r5 = (r >> 3) & 0x1F
    g5 = (g >> 3) & 0x1F
    b5 = (b >> 3) & 0x1F
    t = 0 if a >= 128 else 1
    return f"{(t << 15) | (r5 << 10) | (g5 << 5) | b5:04x}"

def convert_images_to_big_csv(image_paths, output_csv_path):
    with open(output_csv_path, 'w', newline='') as f_out:
        writer = csv.writer(f_out)
        for path in image_paths:
            image = Image.open(path).convert('RGBA')
            if image.size != (16, 16):
                print(f"❌ Skip non-16 ×16 images: {os.path.basename(path)}")
                continue
            for y in range(16):
                row = []
                for x in range(16):
                    row.append(rgba_to_rgb555t(image.getpixel((x, y))))
                writer.writerow(row)
            print(f"✅ has been saved: {os.path.basename(path)}")
    print(f"\n✅ The merge is completed and the output is: {output_csv_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    image_paths = filedialog.askopenfilenames(
        title="Select multiple 16x16 pictures (in order)",
        filetypes=[("PNG Images", "*.png")]
    )

    if not image_paths:
        print("⚠️ No images were selected")
    else:
        output_csv = filedialog.asksaveasfilename(
            title="Save as a large CSV file",
            defaultextension=".csv",
            filetypes=[("CSV files", "*.csv")],
            initialfile="sprites_all.csv"
        )
        if output_csv:
            convert_images_to_big_csv(image_paths, output_csv)
