#ifndef GDISPRITE__
#define GDISPRITE__
#include <Windows.h>
#include "GDIEngine.h"
#include "stb_image.h"

#define GDSPimg_BMP 0
#define GDSPimg_JPG 1

#define MIN(a,b) (((a)<(b))?(a):(b))


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	typedef BOOL(__stdcall* TransParentBitBlt)(
		HDC  hdcDest,
		int  xoriginDest,
		int  yoriginDest,
		int  wDest,
		int  hDest,
		HDC  hdcSrc,
		int  xoriginSrc,
		int  yoriginSrc,
		int  wSrc,
		int  hSrc,
		UINT crTransparent
	);
	extern TransParentBitBlt TransParentBlt;

	//structure to hold the bitmap sprite map
	struct GDSPsprite {
		HBITMAP _spriteMap;

		COLORREF _transparentColour;
		int _transparent;
		
		int* _topLeftX;
		int* _topLeftY;
		int _tileWidth;
		int _tileHeight;
		int _numberOfTiles;
	};

	
	HBITMAP GDSPloadImage(char* fileName, int imgWidth, int imgHeight);
	//GDI SPRITE creation function (each sprite tile must be same width and height)
	struct GDSPsprite GDSPcreateSprite(char* fileName, int imgWidth, int imgHeight, int tileWidth, int tileHeight, int transparent , COLORREF transparentColour);
	int GDSPfreeSprite(struct GDSPsprite* sprite);

	int GDSPdrawSprite(int x, int y, int width, int height, struct GDSPsprite* sprite, int tilePos);



#ifdef __cplusplus
}
#endif // __cplusplus


#endif