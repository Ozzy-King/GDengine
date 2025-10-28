#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#include "GDISprite.h"

HBITMAP GDSPloadImage(char* fileName, int imgWidth, int imgHeight) {
	int width, height, channels;
	unsigned char* data = stbi_load(fileName, &width, &height, &channels, STBI_rgb);

	if (data == NULL) {
		printf("Failed to load image.\n");
		return NULL;
	}
	int rowSize = width * 3;
	for (int i = 0; i < width * height; ++i) {
		unsigned char tmp = data[i * 3];
		data[i * 3] = data[i * 3 + 2];       // Swap Red and Blue
		data[i * 3 + 2] = tmp;
	}

	// Create an HBITMAP from the data
	HBITMAP hBitmap = NULL;
	HDC hdc = GetDC(NULL);
	if (hdc) {
		BITMAPINFO bmi = { 0 };
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = MIN(width, imgWidth);
		bmi.bmiHeader.biHeight = -(MIN(height, imgHeight)); // Negative height for top-down bitmap
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 24;  // 24-bit color
		bmi.bmiHeader.biCompression = BI_RGB;

		// Create the HBITMAP
		hBitmap = CreateDIBitmap(hdc, &bmi.bmiHeader, CBM_INIT, data, &bmi, DIB_RGB_COLORS);
		ReleaseDC(NULL, hdc);
	}

	// Free the image data loaded by stb_image
	stbi_image_free(data);

	return hBitmap;
}

struct GDSPsprite GDSPcreateSprite(char* fileName, int imgWidth, int imgHeight, int tileWidth, int tileHeight, int transparent , COLORREF transparentColour) {
	struct GDSPsprite newSprite;
	newSprite._tileWidth = tileWidth;
	newSprite._tileHeight = tileHeight;
	newSprite._transparent = transparent;
	newSprite._transparentColour = transparentColour;

	//gets sprite into the GDSPsprite
	newSprite._spriteMap = GDSPloadImage(fileName, imgWidth, imgHeight);// (HBITMAP)LoadImage(NULL, ConvertedFilename, IMAGE_BITMAP, imgWidth, imgHeight, LR_LOADFROMFILE);

	//get all the tiles
	int numberOfTiles = (imgWidth / tileWidth) * (imgHeight / tileHeight);//gets number of tiles (will be used later to index arrays)
	newSprite._numberOfTiles = numberOfTiles;//saves for later reference
	newSprite._topLeftX = (int*)malloc(sizeof(int) * numberOfTiles); //allocations space for array
	newSprite._topLeftY = (int*)malloc(sizeof(int) * numberOfTiles); // ditto above
	for (int y = (imgHeight- tileHeight); y >= 0; y -= tileHeight) {
		for (int x = (imgWidth-tileWidth); x >= 0; x -= tileWidth) {
			numberOfTiles--;
			newSprite._topLeftX[numberOfTiles] = x;
			newSprite._topLeftY[numberOfTiles] = y;
		}
	}

	return newSprite;
}

int GDSPfreeSprite(struct GDSPsprite* sprite) {
	free(sprite->_topLeftX);
	free(sprite->_topLeftY);
	DeleteObject(sprite->_spriteMap);
	return 0;
}

int GDSPdrawSprite(int x, int y, int width, int height, struct GDSPsprite* sprite, int tilePos) {
	HDC tempDeviceContext = CreateCompatibleDC(_GDconsoleDeviceContext); //make this a single created varaible that is reused
	HBITMAP temp = SelectObject(tempDeviceContext, sprite->_spriteMap); //links the bitmap to the dc to draw on it
	int newx = x * _GDpixelWidth;
	int newy = y * _GDpixelHeight;
	int newWidth = width * _GDpixelWidth;
	int newHeight = height * _GDpixelHeight;
	if (sprite->_transparent) {
		TransParentBlt(_GDbackBufferDeviceContext, min(newx, _GDrawWidth), min(newy, _GDrawHeight), min(newWidth, _GDrawWidth), min(newHeight, _GDrawHeight), tempDeviceContext, sprite->_topLeftX[tilePos], sprite->_topLeftY[tilePos], sprite->_tileWidth, sprite->_tileHeight, sprite->_transparentColour);
	} else {
		StretchBlt(_GDbackBufferDeviceContext, min(newx, _GDrawWidth), min(newy, _GDrawHeight), min(newWidth, _GDrawWidth), min(newHeight, _GDrawHeight), tempDeviceContext, sprite->_topLeftX[tilePos], sprite->_topLeftY[tilePos], sprite->_tileWidth, sprite->_tileHeight, SRCCOPY);
	}
	SelectObject(tempDeviceContext, temp);
	DeleteDC(tempDeviceContext);
	return 0;

}