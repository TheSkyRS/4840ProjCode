import csv
import tkinter as tk
from tkinter import filedialog

def csv_to_mif(csv_path, mif_path, start_addr_hex="0000"):
    start_addr = int(start_addr_hex, 16)
    with open(csv_path, 'r') as fin:
        data = list(csv.reader(fin))

    with open(mif_path, 'w') as fout:
        fout.write("WIDTH=16;\nDEPTH=256;\nADDRESS_RADIX=HEX;\nDATA_RADIX=HEX;\nCONTENT BEGIN\n")
        for y, row in enumerate(data):
            for x, hexval in enumerate(row):
                addr = start_addr + y * 16 + x
                fout.write(f"{addr:04X} : {hexval.upper()};\n")
        fout.write("END;\n")
    print(f"✅ MIF 写入完成: {mif_path}")

if __name__ == "__main__":
    root = tk.Tk()
    root.withdraw()
    csv_path = filedialog.askopenfilename(filetypes=[("CSV", "*.csv")])
    if csv_path:
        mif_path = csv_path.replace(".csv", ".mif")
        csv_to_mif(csv_path, mif_path, start_addr_hex="0000")
