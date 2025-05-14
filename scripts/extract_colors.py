from PIL import Image
import tkinter as tk
from tkinter import filedialog

def quantize_color(color, threshold=4):
    """Quantize colors by threshold to reduce comparison times"""
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
    root.withdraw()  # Hide main window

    image_path = filedialog.askopenfilename(
        title="Select an image",
        filetypes=[("Image files", "*.jpg *.jpeg *.png *.bmp *.gif")]
    )

    if image_path:
        colors, num_colors = extract_unique_colors(image_path, threshold=4)
        print(f"Number of unique colors: {num_colors}")
        # If you want to view all different colors, uncomment the line below
        # print(colors)
    else:
        print("No file selected.")