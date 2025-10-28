/*
*Version 1.0
*The GDI graphics library with little capability of performance
*Created by Osbourne Laud Clark
**On 6th november 2022
*Treated at the university of plymouth
*
*people downloading this library may modify, use, distribute, and share.
*this must stay in contact and shouldn't be edited  
*/


#ifndef GDIENGINE__
#define GDIENGINE__
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//gets the window handle to the console controlled by this program
//the handle to output to edit the cursor
//get the device contect to draw on

	extern HMODULE _GDhInstance;
	extern HWND _GDconsoleWinHandle; //gets the window handle to the console controlled by this program
	extern HANDLE _GDconsoleOutputHandle; //the handle to output to edit the cursor
	extern HDC _GDconsoleDeviceContext; //get the device contect to draw on

	extern HDC _GDbackBufferDeviceContext;
	extern HBITMAP _GDbackBufferBitMap;

	extern int _GDpixelSetType;
	extern HBRUSH _GDmasterBrush;

	extern RECT _GDrawPixelScreenSize; //raw pixel screen size
	extern int _GDrawWidth, _GDrawHeight;

	extern RECT _GDpixelScreenSize; //user defined pixel screen size
	extern int _GDwidth, _GDheight;

	extern int _GDpixelWidth, _GDpixelHeight;


	//function proto types//

	int _GDstrLen(const char* string);
	int GDinit(int width, int height, int pixelWidth, int pixelHeight, char* title);
	//int GDsetTitle(const char* title);

	/*
	draws to current backbuffer
	returns false(0) if successfull, return true(1) if fails
	*/
	int GDdrawRawPixel(int x, int y, COLORREF colour);
	int GDdrawPixel(int x, int y, COLORREF colour);
	/*
	returns false(0) if successfull, return true(1) if fails
	*/
	int GDclear(COLORREF colour);
	int GDclearBlack(void);

	/*
	the +23 offsets the draw or it will draw under the top bar
	returns false(0) if successfull, return true(1) if fails
	*/
	int GDdrawBackBuffer(void);



	int GDdeInit(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif
