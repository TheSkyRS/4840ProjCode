import csv
import os
import tkinter as tk
from tkinter import filedialog, simpledialog

def write_mif(data_list, width, output_path, start_address=0):
    depth = len(data_list)
    with open(output_path, 'w') as f:
        f.write(f"WIDTH={width};\n")
        f.write(f"DEPTH={depth};\n")
        f.write("ADDRESS_RADIX=HEX;\n")
        f.write("DATA_RADIX=HEX;\n")
        f.write("CONTENT BEGIN\n")
        for i, data in enumerate(data_list):
            addr = start_address + i
            f.write(f"{addr:04X} : {data};\n")
        f.write("END;\n")

def load_tiles_csv(tiles_csv_path):
    """读取tiles.csv，每行拼接为64字符16进制字符串"""
    tiles_data = []
    with open(tiles_csv_path, 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            line = ''.join(hex_str.zfill(4).upper() for hex_str in row)
            tiles_data.append(line)
    return tiles_data

def load_tilemap_csv(tilemap_path):
    """读取tilemap.csv，每元素转换为2位hex字符串"""
    tilemap_data = []
    with open(tilemap_path, 'r') as f:
        reader = csv.reader(f)
        for row in reader:
            for num in row:
                tilemap_data.append(f"{int(num) & 0xFF:02X}")  # 8bit
    return tilemap_data

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()

    print("请选择tiles.csv")
    tiles_csv_path = filedialog.askopenfilename(
        title="选择tiles.csv",
        filetypes=[("CSV files", "*.csv")]
    )
    if not tiles_csv_path:
        print("未选择tiles.csv")
        exit()

    print("请选择tilemap.csv")
    tilemap_csv_path = filedialog.askopenfilename(
        title="选择tilemap.csv",
        filetypes=[("CSV files", "*.csv")]
    )
    if not tilemap_csv_path:
        print("未选择tilemap.csv")
        exit()

    # 询问起始地址
    tile_start = simpledialog.askinteger("Tile MIF起始地址", "请输入tiles.mif的起始地址（十六进制，例如0）", initialvalue=0)
    tilemap_start = simpledialog.askinteger("Tilemap MIF起始地址", "请输入tilemap.mif的起始地址（十六进制，例如0）", initialvalue=0)
    if tile_start is None or tilemap_start is None:
        print("未输入起始地址")
        exit()

    # 加载数据
    tiles_data = load_tiles_csv(tiles_csv_path)
    tilemap_data = load_tilemap_csv(tilemap_csv_path)

    # 输出路径
    dir_name = os.path.dirname(tiles_csv_path)
    tiles_mif_path = os.path.join(dir_name, "tiles.mif")
    tilemap_mif_path = os.path.join(dir_name, "tilemap.mif")

    # 写入mif
    write_mif(tiles_data, 256, tiles_mif_path, start_address=tile_start)
    write_mif(tilemap_data, 8, tilemap_mif_path, start_address=tilemap_start)

    print(f"tiles.mif 保存到: {tiles_mif_path}")
    print(f"tilemap.mif 保存到: {tilemap_mif_path}")