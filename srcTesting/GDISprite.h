#ifndef GDISPRITE__
#define GDISPRITE__
#include <wincodec.h>    // WIC
#include <wincodecsdk.h>

#include <windows.h>     // HBITMAP, etc.
#include <objbase.h>    // _com_error for HRESULT handling
#include "GDIEngine.h"


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
		unsigned int _imgWidth;
		unsigned int _imgHeight;
		HBITMAP _spriteMap;

		COLORREF _transparentColour;
		int _transparent;
		
		int* _topLeftX;
		int* _topLeftY;
		int _tileWidth;
		int _tileHeight;
		int _numberOfTiles;
	};

	int GDSPinit();
	int GDSPdeinit();
	//GDI SPRITE creation function (each sprite tile must be same width and height)
	struct GDSPsprite GDSPcreateSprite(wchar_t* fileName, UINT tileWidth, UINT tileHeight, int transparent , COLORREF transparentColour);
	int GDSPfreeSprite(struct GDSPsprite* sprite);

	int GDSPdrawSprite(int x, int y, int width, int height, struct GDSPsprite* sprite, int tilePos);



#ifdef __cplusplus
}
#endif // __cplusplus


#endif