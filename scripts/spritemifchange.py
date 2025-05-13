import tkinter as tk
from tkinter import filedialog, messagebox

def reverse_sprite_mif(input_path, output_path, group_size=256):
    try:
        with open(input_path, "r") as file:
            lines = file.readlines()

        header_lines = []
        data_lines = []
        in_data = False

        for line in lines:
            if "BEGIN" in line or in_data:
                in_data = True
                data_lines.append(line)
            else:
                header_lines.append(line)

        # Extract the address line (including: and;)
        address_lines = [line for line in data_lines if ":" in line and ";" in line]

        # It is divided into a group of "group_size" rows
        total_groups = len(address_lines) // group_size
        groups = [
            address_lines[i * group_size : (i + 1) * group_size]
            for i in range(total_groups)
        ]

        # Reverse the group order
        reversed_groups = groups[::-1]

        # Regenerate the address
        new_data_lines = []
        for group_index, group in enumerate(reversed_groups):
            for line_index, line in enumerate(group):
                _, value = line.split(":")
                new_address = group_index * group_size + line_index
                new_line = f"{new_address:04X}: {value.strip()}\n"
                new_data_lines.append(new_line)

        # Search for END; line
        end_line = next((l for l in data_lines if "END;" in l), "END;\n")

        # Write to a new file
        with open(output_path, "w") as out_file:
            out_file.writelines(header_lines + ["\n"] + new_data_lines + [end_line])

        messagebox.showinfo("success!", f"Conversion completed!\nd save to ：\n{output_path}")

    except Exception as e:
        messagebox.showerror("error", f"Processing failure:\n{str(e)}")


# ==========================
# GUI 
# ==========================
def run_gui():
    root = tk.Tk()
    root.withdraw()

    input_path = filedialog.askopenfilename(
        title="choose Sprite MIF ",
        filetypes=[("MIF", "*.mif"), ("all files", "*.*")]
    )

    if not input_path:
        messagebox.showinfo("cancel", "The input file was not selected.")
        return

    output_path = filedialog.asksaveasfilename(
        title="choose the path",
        defaultextension=".mif",
        filetypes=[("MIF", "*.mif"), ("all files", "*.*")]
    )

    if not output_path:
        messagebox.showinfo("cancel", "The output file was not selected")
        return

    reverse_sprite_mif(input_path, output_path)

if __name__ == "__main__":
    run_gui()
