import tkinter as tk
from tkinter import filedialog, messagebox

def reverse_tile_groups(input_file_path, output_file_path):
    """
    Group each row of 64-bit data in the MIF file into groups of 4 bits and rearrange each group from right to left.
    The address, header comments and other contents remain unchanged.
    """
    try:
        with open(input_file_path, "r") as file:
            lines = file.readlines()

        header_lines = []
        data_lines = []
        in_data = False

        for line in lines:
            stripped = line.strip()
            if stripped.lower().startswith("content") or in_data:
                in_data = True
                data_lines.append(line)
            else:
                header_lines.append(line)

        transformed_data_lines = []
        for line in data_lines:
            if ":" not in line:
                transformed_data_lines.append(line)
                continue

            address, values = line.split(":")
            values = values.strip().rstrip(";")

            groups = [values[i:i+4] for i in range(0, len(values), 4)]

            if len(groups) == 16:
                reversed_groups = groups[::-1]
                new_value = ''.join(reversed_groups)
                transformed_data_lines.append(f"{address.strip()}: {new_value};\n")
            else:
                transformed_data_lines.append(line)

        with open(output_file_path, "w") as out_file:
            out_file.writelines(header_lines + transformed_data_lines)

        messagebox.showinfo("success", f"The conversion is complete. The file is saved to：\n{output_file_path}")

    except Exception as e:
        messagebox.showerror("error", f"error：\n{str(e)}")

# ---------------- GUI  ----------------
def run_gui():
    root = tk.Tk()
    root.withdraw()  # The main window is not displayed

    input_path = filedialog.askopenfilename(
        title="please choose MIF ",
        filetypes=[("MIF ", "*.mif"), ("all files", "*.*")]
    )

    if not input_path:
        messagebox.showinfo("cancel", "not choose files ")
        return

    output_path = filedialog.asksaveasfilename(
        title="please chooe the save file",
        defaultextension=".mif",
        filetypes=[("MIF ", "*.mif"), ("all files", "*.*")]
    )

    if not output_path:
        messagebox.showinfo("cancel", "not choose files ")
        return

    reverse_tile_groups(input_path, output_path)

if __name__ == "__main__":
    run_gui()
