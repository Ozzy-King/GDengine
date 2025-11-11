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
//#include <stdio.h>

//NEED TO ADD FELAPSETIME ;)

HMODULE _GDhInstance = NULL;
HWND _GDconsoleWinHandle = NULL; //gets the window handle to the console controlled by this program

HANDLE _GDwindowThread = NULL;
DWORD _GDwindowThreadId;
HANDLE _GDwindowReadyEvent; //used to feecitly wait for window startup
int _GDwindowOpen = 0;

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

//used by the window to handle incoming messages
LRESULT CALLBACK _GDwndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
			_GDwindowOpen = 0;
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

//ran in thead so the seperate thread owns the created window and is 
DWORD WINAPI _GDcreateWindow(LPVOID lpParam){
	_GDhInstance = GetModuleHandle(NULL);
	const wchar_t winClassName[] = L"_GDGameWindow";
	//printf("got hinstance\n");
	
	WNDCLASSW wc = {};
	wc.lpfnWndProc = _GDwndProc;
	wc.hInstance = _GDhInstance;
	wc.lpszClassName = winClassName;
	wc.style = CS_OWNDC;
	
	RegisterClassW(&wc);
	//printf("registered window class\n");
	
	RECT _GDfullWindowSize = {0}; //size of entire window
	_GDfullWindowSize.left = _GDfullWindowSize.top = 0;
	_GDfullWindowSize.right = _GDrawWidth;
	_GDfullWindowSize.bottom = _GDrawHeight;
	AdjustWindowRectEx(&_GDfullWindowSize,WS_OVERLAPPEDWINDOW,FALSE,0);
	
	_GDconsoleWinHandle = CreateWindowExW(
		0,                              // Optional window styles.
		winClassName,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 
		_GDfullWindowSize.right - _GDfullWindowSize.left, _GDfullWindowSize.bottom - _GDfullWindowSize.top,

		NULL,       // Parent window    
		NULL,       // Menu
		_GDhInstance,  // Instance handle
		NULL        // Additional application data
		);
	//printf("created window\n");
	
	ShowWindow(_GDconsoleWinHandle, SW_SHOW);
	_GDconsoleDeviceContext = GetDC(_GDconsoleWinHandle); //get console device contect handle (for drawing)
	//printf("window showed\n");
	
	SetEvent(_GDwindowReadyEvent);
	_GDwindowOpen = 1;
	//message loop for the newly created window
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}


/*
returns false(0) if successfull, return true(1) if fails
title needs to be less than 63998 to be less than 64000 characters long and accomodate for terminating('\0') value
*/
int GDinit(int width, int height, int pixelWidth, int pixelHeight, wchar_t* title) {

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
	//printf("setup passed dimenstions\n");
	
	//create dediceated window for graphics
	
	//create extra buffer for displaying and tings
	_GDwindowReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	_GDwindowThread = CreateThread(NULL, 0, _GDcreateWindow, NULL, 0, &_GDwindowThreadId);
	WaitForSingleObject(_GDwindowReadyEvent, INFINITE);
	CloseHandle(_GDwindowReadyEvent);
	
	//_GDcreateWindow();
	//printf("created window\n");
	
	//creates double buffer (need to delete these as i created them, they're not from the console it self )
	_GDbackBufferDeviceContext = CreateCompatibleDC(_GDconsoleDeviceContext);
	_GDbackBufferBitMap = CreateCompatibleBitmap(_GDconsoleDeviceContext, _GDrawWidth, _GDrawHeight);
	SelectObject(_GDbackBufferDeviceContext, _GDbackBufferBitMap); //links the bitmap to the dc to draw on it
	//printf("setup back buffer\n");

	//lock widnow from drawing ----------- test

	_GDpixelSetType = GetDeviceCaps(_GDbackBufferDeviceContext, RC_BITBLT);

	//edits the title
	GDsetTitle(title);

	//this is for tranparentBlt
	//loading important dll :)
	__SMimg32DLL = LoadLibraryA("./msimg32.dll");
	TransParentBlt = (TransParentBitBlt)GetProcAddress(__SMimg32DLL, "TransparentBlt");
	//printf("got blt compatibility and loaded libaries\n");

	return 0;
}

/*
max title length is 255 character exculiding the termination char
supports 
*/
inline int GDsetTitle(wchar_t* title) {
	return SetWindowTextW(_GDconsoleWinHandle, title);
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
		//printf("Out Of Range Pixel Draw!\n");
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
		//printf("Out Of Range Pixel Draw!\n");
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
	if (BitBlt(_GDconsoleDeviceContext, 0, 0, _GDrawWidth, _GDrawHeight, _GDbackBufferDeviceContext, 0, 0, SRCCOPY)) {
		return 0;
	}
	else {
		//printf("Back Buffer Blit failed\n");
		return 1;
	}
	LockWindowUpdate(_GDconsoleWinHandle);
}

int GDdeInit() {
	SendMessage(_GDconsoleWinHandle, WM_QUIT, 0, 0);
	WaitForSingleObject(_GDwindowThread, INFINITE);
    CloseHandle(_GDwindowThread);
	//printf("closed thread");
	
	DeleteObject(_GDbackBufferBitMap);
	DeleteDC(_GDbackBufferDeviceContext);
	ReleaseDC(_GDconsoleWinHandle, _GDconsoleDeviceContext);
	FreeLibrary(__SMimg32DLL);
	//printf("freed extra resources");
	return 0;
}