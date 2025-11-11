# GDengine
## About

A windows based ultra light weight graphics/game engine for 2D rendering. using the DLLs found in windows this library can achieve small compile sizes. it abstracts away boilerplate and low level understanding of the windows system allowing for an easy utilisation and programming.
a hello world program which consists of random points being plotted on the window comes to roughly 17KB in size.

## features
### Base Engine
- Create a dedicated window
- Doubled buffer for faster drawing routines
- Basic pixel drawing (points, Fullscreen)
- Unicode supported window title
### Sprite
- load images of different formats
|IMAGE TYPE|BMP|GIF|ICO|JPEG|PNG|TIFF|
|-|-|-|-|-|-|-|
|SUPPORTED|✅|✅|✅|✅|✅|✅|
- basic tile based image loading
- draw loaded images to buffer quickly

## Base Engine
### Global Variables
```c
	int _GDwindowOpen;//indicator to window state

	int _GDpixelWidth, _GDpixelHeight;//pixel dim

	int _GDwidth, _GDheight;//window dim
	int _GDrawWidth, _GDrawHeight;//screen dim * pixel dim
```

### Functions
#### int GDinit(int width, int height, int pixelWidth, int pixelHeight, wchar_t* title);
intilises the graphics window 
- width: number of columns of pixels
- height: number of rows of pixels
- pixelWidth: width of each pixel
- pixelHeight: height of each pixel
- title: title of the window (e.g. L"hello world")

#### int GDsetTitle(wchar_t* title);
set the title of the window
- title: title of the window (e.g. L"hello world")

#### GDisKeyPressed(VK_KEY) and GDisKeyReleased(VK_KEY)
returns if a key has been pressed or released 
- VK_KEY: keys defined by windows
	- [Virtual-Key Codes (Winuser.h) - Win32 apps | Microsoft Learn](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)

#### int GDdrawRawPixel(int x, int y, COLORREF colour);
draws a pixel based on the screens pixels
- x: position along x to draw
- y: position along y to draw
- colour: colour to use when drawing 

#### int GDdrawPixel(int x, int y, COLORREF colour);
draws a pixel based pixel width and height
- x: position along x to draw
- y: position along y to draw
- colour: colour to use when drawing 

#### int GDclear(COLORREF colour);
clear the entire screen to defined COLORREF

- colour: colour to fill the screen with
#### int GDclearBlack(void);
clear the window to black (RGB - 0x0, 0x0, 0x0)

#### int GDdrawBackBuffer(void);
draws the back buffer to the the window
#### int GDdeInit(void);
deinitilises window and free resources.

## Sprite Engine
### Custom Struct
```c
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
```

### Functions
#### int GDSPinit();
itilise the sprite loading engine
#### int GDSPdeinit();
deinitilise the sprite engine(free resources)
#### struct GDSPsprite GDSPcreateSprite(wchar_t* fileName, UINT tileWidth, UINT tileHeight, int transparent , COLORREF transparentColour);
load in new images for use in rendering
- fileName: name of the file
- tileWidth: each tiles width. if non, set to the image width
- tileHeight: each tiles height. if non, set to the image height
- trasnparent: if there are transparent pixels
- transparentColour: colour pf pixel to be transparent

#### int GDSPfreeSprite(struct GDSPsprite* sprite);
destroy sprite structure(free resources)
#### int GDSPdrawSprite(int x, int y, int width, int height, struct GDSPsprite* sprite, int tilePos);
draws sprite to back buffer (requires the use of the GDengine)
- x: coord of top left corner
- y: coord of top left corner 
- width: width for the desired sprite(stretch)
- height: height for the desired sprite(stretch)
- sprite: pointer to sprite to use
- tilepos: the tiles with in the sprite to use. if non, set 0
