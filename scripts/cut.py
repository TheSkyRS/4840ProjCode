from PIL import Image
import os
import tkinter as tk
from tkinter import filedialog

def slice_horizontal_tiles(image_path):
    img = Image.open(image_path).convert('RGBA')
    width, height = img.size
    tile_size = height  # The width and height are equal, and the height is used as the tile width

    tile_count = width // tile_size
    base_name = os.path.splitext(os.path.basename(image_path))[0]
    output_dir = os.path.join(os.path.dirname(image_path), f"{base_name}_tiles")
    os.makedirs(output_dir, exist_ok=True)

    for i in range(tile_count):
        tile = img.crop((i * tile_size, 0, (i + 1) * tile_size, height))
        tile.save(os.path.join(output_dir, f"tile_{i:02d}.png"))
        print(f"✅ save tile_{i:02d}.png")

    print(f"\n🎉 save {tile_count} tile to: {output_dir}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    image_path = filedialog.askopenfilename(title="Select a tile material image", filetypes=[("PNG", "*.png")])
    if image_path:
        slice_horizontal_tiles(image_path)
    else:
        print("⚠️ Unselected image")
