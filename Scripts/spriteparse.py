from PIL import Image
import sys
im = Image.open("fsociety40.jpg")
pix = im.load()
for r in range(0,40):
	for c in range(0,40):
		if(pix[r,c][0] > 127):
			sys.stdout.write(str(hex(255))+',')
		else:
			sys.stdout.write(str(hex(0))+',')