import csv
import tkinter as tk
from tkinter import filedialog, simpledialog

def csv_to_tiles_mif():
    root = tk.Tk()
    root.withdraw()

    # choose tiles.csv 
    csv_path = filedialog.askopenfilename(
        title="choose tiles.csv document",
        filetypes=[("CSV files", "*.csv")]
    )
    if not csv_path:
        print("❌ no choose CSV document")
        return

    mif_path = filedialog.asksaveasfilename(
        title="save as tiles.mif ",
        defaultextension=".mif",
        filetypes=[("MIF files", "*.mif")],
        initialfile="tiles.mif"
    )
    if not mif_path:
        print("❌ no choose path")
        return

    start_hex = simpledialog.askstring("initial address", "please input start addrsss（hex，like 07D0）", initialvalue="0000")
    if not start_hex:
        print("❌ no choose address")
        return
    start_addr = int(start_hex, 16)

    with open(csv_path, 'r') as f:
        rows = list(csv.reader(f))

    total_lines = len(rows)
    depth = total_lines

    with open(mif_path, 'w') as f:
        f.write("WIDTH=256;\n")
        f.write(f"DEPTH={depth};\n")
        f.write("ADDRESS_RADIX=HEX;\n")
        f.write("DATA_RADIX=HEX;\n")
        f.write("CONTENT BEGIN\n")
        for i, row in enumerate(rows):
            addr = start_addr + i
            line = ''.join(cell.zfill(4).upper() for cell in row)
            f.write(f"{addr:04X} : {line};\n")
        f.write("END;\n")

    print(f"✅ output tiles.mif：{mif_path}")

if __name__ == "__main__":
    csv_to_tiles_mif()
