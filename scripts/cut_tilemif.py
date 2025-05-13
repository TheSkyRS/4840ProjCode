import csv
import tkinter as tk
from tkinter import filedialog

def convert_tilecsv_to_mif(csv_path, mif_path, start_address=0):
    with open(csv_path, 'r') as f:
        reader = csv.reader(f)
        tile_rows = list(reader)

    depth = len(tile_rows)
    with open(mif_path, 'w') as fout:
        fout.write("WIDTH=256;\n")
        fout.write(f"DEPTH={depth};\n")
        fout.write("ADDRESS_RADIX=HEX;\n")
        fout.write("DATA_RADIX=HEX;\n")
        fout.write("CONTENT BEGIN\n")
        for i, row in enumerate(tile_rows):
            hex_line = ''.join(val.zfill(4).upper() for val in row)
            fout.write(f"{start_address + i:04X} : {hex_line};\n")
        fout.write("END;\n")
    print(f"✅ tiles.mif 生成成功：{mif_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    csv_path = filedialog.askopenfilename(title="选择 tile.csv", filetypes=[("CSV", "*.csv")])
    if not csv_path:
        print("❌ 未选择 CSV 文件")
        exit()

    mif_path = filedialog.asksaveasfilename(title="保存 tiles.mif", defaultextension=".mif")
    if not mif_path:
        print("❌ 未指定 MIF 保存路径")
        exit()

    convert_tilecsv_to_mif(csv_path, mif_path)
