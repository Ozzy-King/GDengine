/*
*Version 1.0
*The GDI graphics library with little capability of performance
*Created by Osbourne Laud Clark
**On 6th november 2022
*Created at the university of plymouth
*
*people downloading this library may modify, use, distribute, and share.
*this must stay in contact and shouldn't be edited  
*/



#include "GDIEngine.h"
#include <libloaderapi.h>
#include <stdio.h>

//NEED TO ADD FELAPSETIME ;)

HMODULE _GDhInstance = NULL;
HWND _GDconsoleWinHandle = NULL; //gets the window handle to the console controlled by this program
HANDLE _GDconsoleOutputHandle = NULL; //the handle to output to edit the cursor
HDC _GDconsoleDeviceContext = NULL; //get the device contect to draw on

HDC _GDbackBufferDeviceContext = NULL;
HBITMAP _GDbackBufferBitMap = NULL;

int _GDpixelSetType = 0;
HBRUSH _GDmasterBrush = NULL;

RECT _GDrawPixelScreenSize = {0}; //raw pixel screen size
int _GDrawWidth = 0, _GDrawHeight = 0;

RECT _GDpixelScreenSize = {0}; //user defined pixel screen size
int _GDwidth = 0, _GDheight = 0;

int _GDpixelWidth = 0, _GDpixelHeight = 0;

//this is soem messed up stuff about to happen
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
HMODULE __SMimg32DLL;
TransParentBitBlt TransParentBlt = NULL;


/*
gets the length of and inputed string (assumes it has null terminator("\0"))
*/
int _GDstrLen(const char* string) {
	int i = 0;
	while (string[i]!='\0') {
		i++;
	}
	return i;
}


int _GDcreateWindow(){
	_GDhInstance = GetModuleHandle(NULL);
	const wchar_t winClassName[] = L"_GDGameWindow";
	
	WNDCLASS wc = {};
	wc.lpfnWndProc = NULL;
	wc.hInstance = _GDhInstance;
	wc.lpszClassName = winClassName;
	wc.style = CS_OWNDC;
	
	RegisterClass(&wc);
	
	_GDconsoleWinHandle = CreateWindowEx(
		0,                              // Optional window styles.
		winClassName,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, _GDrawWidth, _GDrawHeight,

		NULL,       // Parent window    
		NULL,       // Menu
		_GDhInstance,  // Instance handle
		NULL        // Additional application data
		);

	ShowWindow(_GDconsoleWinHandle, SW_SHOW);
	_GDconsoleDeviceContext = GetDC(_GDconsoleWinHandle); //get console device contect handle (for drawing)

}


/*
returns false(0) if successfull, return true(1) if fails
title needs to be less than 63998 to be less than 64000 characters long and accomodate for terminating('\0') value
*/
int GDinit(int width, int height, int pixelWidth, int pixelHeight, char* title) {

	//saves the width and height aswell as raw pixel width and height for later use
	_GDwidth = width;
	_GDheight = height;
	_GDrawWidth = width * pixelWidth;
	_GDrawHeight = height * pixelHeight;

	_GDpixelScreenSize.left = _GDpixelScreenSize.top = 0;
	_GDpixelScreenSize.right = _GDwidth;
	_GDpixelScreenSize.bottom = _GDheight;

	_GDrawPixelScreenSize.left = _GDrawPixelScreenSize.top = 0;
	_GDrawPixelScreenSize.right = _GDrawWidth;
	_GDrawPixelScreenSize.bottom = _GDrawHeight;

	_GDpixelWidth = pixelWidth;
	_GDpixelHeight = pixelHeight;

	//create dediceated window for graphics
	_GDhInstance = GetModuleHandle(NULL);
	
	//create extra buffer for displaying and tings
	_GDcreateWindow();
	
	
	//creates double buffer (need to delete these as i created them, they're not from the console it self )
	_GDbackBufferDeviceContext = CreateCompatibleDC(_GDconsoleDeviceContext);
	_GDbackBufferBitMap = CreateCompatibleBitmap(_GDconsoleDeviceContext, _GDrawWidth, _GDrawHeight);
	SelectObject(_GDbackBufferDeviceContext, _GDbackBufferBitMap); //links the bitmap to the dc to draw on it

	//lock widnow from drawing ----------- test

	_GDpixelSetType = GetDeviceCaps(_GDbackBufferDeviceContext, RC_BITBLT);

	//edits the title
	GDsetTitle(title);

	//this is for tranparentBlt
	//loading important dll :)
	__SMimg32DLL = LoadLibraryA("./msimg32.dll");
	TransParentBlt = (TransParentBitBlt)GetProcAddress(__SMimg32DLL, "TransparentBlt");

	return 0;
}

/*
* note -- says some errors about buffer over runs and address null but should never happen
returns false(0) if successfull, return true(1) if fails
changes the title to the given string or to "GDIEngine-Tile" by defalut
title needs to be less than 63998 to be less than 64000 characters long and accomodate for terminating('\0') value
*/
int GDsetTitle(const char* title) {
	int replace = 0;
	char replacementText[] = "Error Occured while setting title (eather too small or big)";
	char* origTitle = title;
	int titleLen = 0;

	//error checking	
	if (title == NULL) {
		replace = 1; 
		title = replacementText; 
	}
	titleLen = _GDstrLen(title);
	if (titleLen <= 0 || titleLen >= 63998) {
		replace = 1;
		title = replacementText;
		titleLen = _GDstrLen(title);
	}

	WCHAR* titleTextPointer = (WCHAR*)malloc(sizeof(WCHAR) * (titleLen + 1)); //creates a new wide char list(WCHAR array) +1 for a terminating character(\0)
	if (titleTextPointer == NULL) { printf("error occured while locating space\n"); return 1; }
	for (int i = 0; i < titleLen; i++) {//iterats through lists and transfers memory
		titleTextPointer[i] = title[i]; //actual transfer (char is 1byte, wide char is 2 bytes)
	}
	titleTextPointer[titleLen] = '\0'; //adds the last terminating value to properly use the widechar in function
	SetConsoleTitle((LPCWSTR)titleTextPointer); //converts the WCHAR* to LPCWSTR=(long pointer constant wide string)
	free(titleTextPointer); //deletes dynamically allocated array

	if (replace) {
		title = origTitle;
	}
	return 0;
}

/*
draws to current backbuffer
returns false(0) if successfull, return true(1) if fails
*/
int GDdrawRawPixel(int x, int y, COLORREF colour) {
	if (x < _GDrawWidth && y < _GDrawHeight) {
		!_GDpixelSetType ? SetPixelV(_GDbackBufferDeviceContext, x, y, colour) : SetPixel(_GDbackBufferDeviceContext, x, y, colour);
	}
	else {
		printf("Out Of Range Pixel Draw!\n");
		return 1;
	}
	return 0;
}
int GDdrawPixel(int x, int y, COLORREF colour) {
	if (x < _GDwidth && y < _GDheight) {
		int newx = x * _GDpixelWidth, newy = y * _GDpixelHeight;
		RECT settingPixel;
		settingPixel.left = newx; settingPixel.top = newy;
		settingPixel.right = newx + _GDpixelWidth; settingPixel.bottom = newy + _GDpixelHeight;

		_GDmasterBrush = CreateSolidBrush(colour);
		FillRect(_GDbackBufferDeviceContext, &settingPixel, _GDmasterBrush);
		DeleteObject(_GDmasterBrush);
	}
	else {
		printf("Out Of Range Pixel Draw!\n");
		return 1;
	}
	return 0;
}

/*
returns false(0) if successfull, return true(1) if fails
*/
int GDclear(COLORREF colour) {
	_GDmasterBrush = CreateSolidBrush(colour);
	FillRect(_GDbackBufferDeviceContext, &_GDrawPixelScreenSize, _GDmasterBrush);
	DeleteObject(_GDmasterBrush);
	return 0;
}
int GDclearBlack() {
	GDclear((COLORREF)RGB(0, 0, 0));
	return 0;
}

/*
the ,+31 on y, +8 on x, weird offsets the draw or it will draw under the top bar
returns false(0) if successfull, return true(1) if fails
*/
int GDdrawBackBuffer() {
	LockWindowUpdate(NULL);
	if (BitBlt(_GDconsoleDeviceContext, 0 + 8, 0 + 31, _GDrawWidth, _GDrawHeight, _GDbackBufferDeviceContext, 0, 0, SRCCOPY)) {
		return 0;
	}
	else {
		printf("Back Buffer Blit failed\n");
		return 1;
	}
	LockWindowUpdate(_GDconsoleWinHandle);
}

int GDdeInit() {
	DeleteObject(_GDbackBufferBitMap);
	DeleteDC(_GDbackBufferDeviceContext);
	ReleaseDC(_GDconsoleWinHandle, _GDconsoleDeviceContext);
	FreeLibrary(__SMimg32DLL);
	return 0;
}