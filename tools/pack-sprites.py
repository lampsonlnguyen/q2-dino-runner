"""Pack the upstream grayscale sprite sheet as two palette indices per byte."""
import sys

from PIL import Image

image = Image.open(sys.argv[1])
if image.mode != "L" or image.size != (1233, 68) or image.info.get("transparency") != 0:
    raise SystemExit("Unexpected Dino Runner sprite sheet")
pixels = image.tobytes()
palette = sorted(set(pixels))
if len(palette) > 16:
    raise SystemExit("Sprite sheet has more than 16 shades")
indices = [palette.index(pixel) for pixel in pixels]
packed = [(indices[i] | indices[i + 1] << 4) for i in range(0, len(indices), 2)]
print("enum { SHEET_WIDTH = 1233, SHEET_HEIGHT = 68 };")
print("static const unsigned char palette[] = {" + ", ".join(map(str, palette)) + "};")
print("static const unsigned char sheet[] = {")
for row in range(0, len(packed), 20):
    print("    " + ", ".join(f"0x{b:02x}" for b in packed[row:row + 20]) + ",")
print("};")
