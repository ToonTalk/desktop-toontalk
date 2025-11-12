#!/usr/bin/env python3
"""Convert BMP files to PNG with transparency for web browser compatibility"""

from PIL import Image
from collections import deque

def flood_fill_from_borders(img, target_color=(0, 0, 0)):
    """
    Flood fill transparency from all borders of the image.
    Used for robots where only black connected to borders should be transparent.
    """
    pixels = img.load()
    width, height = img.size
    visited = set()

    # Queue for flood fill (x, y)
    queue = deque()

    # Add all border pixels that match target color to queue
    # Top and bottom borders
    for x in range(width):
        if pixels[x, 0][:3] == target_color:
            queue.append((x, 0))
        if pixels[x, height-1][:3] == target_color:
            queue.append((x, height-1))

    # Left and right borders
    for y in range(height):
        if pixels[0, y][:3] == target_color:
            queue.append((0, y))
        if pixels[width-1, y][:3] == target_color:
            queue.append((width-1, y))

    # Flood fill
    while queue:
        x, y = queue.popleft()

        if (x, y) in visited:
            continue

        if x < 0 or x >= width or y < 0 or y >= height:
            continue

        if pixels[x, y][:3] != target_color:
            continue

        visited.add((x, y))
        r, g, b, a = pixels[x, y]
        pixels[x, y] = (r, g, b, 0)  # Make transparent

        # Add neighbors
        for dx, dy in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
            nx, ny = x + dx, y + dy
            if (nx, ny) not in visited:
                queue.append((nx, ny))

    return img

def convert_with_transparency(bmp_path, png_path, transparency_mode='all_black'):
    """
    Convert BMP to PNG with transparency.

    transparency_mode:
    - 'all_black': All black pixels become transparent
    - 'border_black': Only black pixels connected to borders become transparent
    - 'none': No transparency
    """
    print(f"Converting {bmp_path} -> {png_path} (mode: {transparency_mode})...")

    try:
        img = Image.open(bmp_path).convert('RGBA')

        if transparency_mode == 'all_black':
            # Convert all black pixels to transparent
            pixels = img.load()
            width, height = img.size
            for y in range(height):
                for x in range(width):
                    r, g, b, a = pixels[x, y]
                    if r == 0 and g == 0 and b == 0:
                        pixels[x, y] = (r, g, b, 0)  # Make transparent

        elif transparency_mode == 'border_black':
            # Only black pixels connected to borders become transparent
            img = flood_fill_from_borders(img, target_color=(0, 0, 0))

        img.save(png_path, 'PNG')
        print(f"  ✓ Success ({img.size[0]}x{img.size[1]} pixels)")

    except Exception as e:
        print(f"  ✗ Failed: {e}")

# Sprite files with their transparency modes
sprites = [
    ('public/assets/sprites/bird.bmp', 'public/assets/sprites/bird.png', 'all_black'),
    ('public/assets/sprites/box.bmp', 'public/assets/sprites/box.png', 'all_black'),  # scale
    ('public/assets/sprites/nest.bmp', 'public/assets/sprites/nest.png', 'all_black'),
    ('public/assets/sprites/number.bmp', 'public/assets/sprites/number.png', 'all_black'),
    ('public/assets/sprites/text.bmp', 'public/assets/sprites/text.png', 'all_black'),
    ('public/assets/sprites/robot.bmp', 'public/assets/sprites/robot.png', 'border_black'),
    ('public/assets/sprites/wand.bmp', 'public/assets/sprites/wand.png', 'all_black'),
]

# Background file (no transparency)
backgrounds = [
    ('public/assets/backgrounds/city.bmp', 'public/assets/backgrounds/city.png', 'none'),
]

all_files = sprites + backgrounds

for bmp_path, png_path, mode in all_files:
    convert_with_transparency(bmp_path, png_path, mode)

print("\n✅ Conversion complete!")
print("BMP files have been converted to PNG with proper transparency")
