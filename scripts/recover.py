from PIL import Image
import csv
import tkinter as tk
from tkinter import filedialog
import os

def rgb555t_to_rgba(hex_str):
    val = int(hex_str, 16)
    t_bit = (val >> 15) & 0x1
    r = ((val >> 10) & 0x1F) << 3
    g = ((val >> 5) & 0x1F) << 3
    b = (val & 0x1F) << 3
    a = 0 if t_bit == 1 else 255
    return (r, g, b, a)

def csv_to_image(csv_path, output_path=None):
    with open(csv_path, 'r') as f:
        reader = csv.reader(f)
        rows = list(reader)

    if len(rows) != 16 or any(len(row) != 16 for row in rows):
        raise ValueError("The CSV file should be 16×16")

    image = Image.new('RGBA', (16, 16))
    for y in range(16):
        for x in range(16):
            image.putpixel((x, y), rgb555t_to_rgba(rows[y][x]))

    if output_path is None:
        output_path = csv_path.replace('.csv', '_preview.png')

    image.save(output_path)
    print(f"✅ The image preview has been saved：{output_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    csv_path = filedialog.askopenfilename(filetypes=[("CSV Files", "*.csv")])
    if csv_path:
        csv_to_image(csv_path)
    else:
        print("The CSV file was not selected")
