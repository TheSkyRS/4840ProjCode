import csv
import tkinter as tk
from tkinter import filedialog, simpledialog

def csv_to_mif_gui():
    root = tk.Tk()
    root.withdraw()

    csv_path = filedialog.askopenfilename(
        title="choose sprites_all.csv ",
        filetypes=[("CSV files", "*.csv")]
    )
    if not csv_path:
        print("❌ not choose CSV ")
        return

    mif_path = filedialog.asksaveasfilename(
        title="save MIF ",
        defaultextension=".mif",
        filetypes=[("MIF files", "*.mif")],
        initialfile="sprites_all.mif"
    )
    if not mif_path:
        print("❌ No save path was specified")
        return

    start_hex = simpledialog.askstring("orginal address", "Please enter the starting address (hexadecimal)", initialvalue="0000")
    if not start_hex:
        print("❌ No address entered")
        return
    base = int(start_hex, 16)

    
    with open(csv_path, 'r') as f:
        data = list(csv.reader(f))

    
    with open(mif_path, 'w') as fout:
        fout.write("WIDTH=16;\n")
        fout.write(f"DEPTH={len(data) * 16};\n")
        fout.write("ADDRESS_RADIX=HEX;\n")
        fout.write("DATA_RADIX=HEX;\n")
        fout.write("CONTENT BEGIN\n")
        for i, row in enumerate(data):
            for j, val in enumerate(row):
                addr = base + i * 16 + j
                fout.write(f"{addr:04X} : {val.upper()};\n")
        fout.write("END;\n")

    print(f"✅ save mif：{mif_path}")

if __name__ == "__main__":
    csv_to_mif_gui()
