#!/usr/bin/env python3
"""Convert BMP files to PNG for web browser compatibility"""

from PIL import Image
import os

# Sprite files to convert
sprites = [
    ('public/assets/sprites/box.bmp', 'public/assets/sprites/box.png'),
    ('public/assets/sprites/nest.bmp', 'public/assets/sprites/nest.png'),
    ('public/assets/sprites/pumpy.bmp', 'public/assets/sprites/pumpy.png'),
    ('public/assets/sprites/texpad.bmp', 'public/assets/sprites/texpad.png'),
    ('public/assets/sprites/robot.bmp', 'public/assets/sprites/robot.png'),
]

# Background file to convert
backgrounds = [
    ('public/assets/backgrounds/city.bmp', 'public/assets/backgrounds/city.png'),
]

all_files = sprites + backgrounds

for bmp_path, png_path in all_files:
    try:
        print(f"Converting {bmp_path} -> {png_path}...")
        img = Image.open(bmp_path)
        img.save(png_path, 'PNG')
        print(f"  ✓ Success ({img.size[0]}x{img.size[1]} pixels)")
    except Exception as e:
        print(f"  ✗ Failed: {e}")

print("\n✅ Conversion complete!")
print("BMP files have been converted to PNG for browser compatibility")
