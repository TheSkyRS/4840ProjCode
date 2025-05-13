import csv
import tkinter as tk
from tkinter import filedialog, simpledialog

def tilemap_csv_to_mif():
    root = tk.Tk()
    root.withdraw()

    #  tilemap.csv
    csv_path = filedialog.askopenfilename(
        title="choose tilemap.csv",
        filetypes=[("CSV files", "*.csv")]
    )
    if not csv_path:
        print("❌ not choose CSV document")
        return

    #
    start_hex = simpledialog.askstring("orignial address", "please input the start address（hex）", initialvalue="04B0")
    if not start_hex:
        print("❌ not choose input address ")
        return
    start_addr = int(start_hex, 16)

    # 
    mif_path = filedialog.asksaveasfilename(
        title=" tilemap.mif",
        defaultextension=".mif",
        filetypes=[("MIF files", "*.mif")],
        initialfile="tilemap2.mif"
    )
    if not mif_path:
        print("❌ no choosing path")
        return

    with open(csv_path, 'r') as f:
        reader = csv.reader(f)
        flat_ids = []
        for row in reader:
            flat_ids.extend([int(x) for x in row])

    
    with open(mif_path, 'w') as fout:
        fout.write("WIDTH=8;\n")
        fout.write(f"DEPTH={len(flat_ids)};\n")
        fout.write("ADDRESS_RADIX=HEX;\n")
        fout.write("DATA_RADIX=HEX;\n")
        fout.write("CONTENT BEGIN\n")
        for i, val in enumerate(flat_ids):
            fout.write(f"{start_addr + i:04X} : {val:02X};\n")
        fout.write("END;\n")

    print(f"✅ save mif to: {mif_path}")

if __name__ == "__main__":
    tilemap_csv_to_mif()
