import os
import zipfile
import struct

reales_dir = r"c:\Users\20080\OneDrive\Рабочий стол\reales"
labor_dir = r"c:\Users\20080\OneDrive\Рабочий стол\labor"
desktop_dir = r"c:\Users\20080\OneDrive\Рабочий стол"

loader_exe = os.path.join(labor_dir, "loader_raw.exe")
temp_zip = os.path.join(labor_dir, "reales_bundle.zip")
out_exe = os.path.join(desktop_dir, "HeterojunctionLab_Single.exe")

print("1. Creating compressed ZIP bundle from reales...")
with zipfile.ZipFile(temp_zip, 'w', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as z:
    for root, dirs, files in os.walk(reales_dir):
        for f in files:
            full_path = os.path.join(root, f)
            rel_path = os.path.relpath(full_path, reales_dir)
            z.write(full_path, rel_path)

zip_size = os.path.getsize(temp_zip)
print(f"ZIP Bundle created: {zip_size:,} bytes")

print("2. Assembling final Single-Executable file...")
MAGIC = b"PKGBNDL1"
footer = MAGIC + struct.pack("<Q", zip_size)

with open(out_exe, "wb") as f_out:
    # 1. Загрузчик
    with open(loader_exe, "rb") as f_loader:
        f_out.write(f_loader.read())
    # 2. ZIP-архив
    with open(temp_zip, "rb") as f_zip:
        f_out.write(f_zip.read())
    # 3. 16-байтный футер
    f_out.write(footer)

final_size = os.path.getsize(out_exe)
print(f"SUCCESS! Output executable: {out_exe}")
print(f"Total size: {final_size:,} bytes ({final_size / (1024*1024):.2f} MB)")
