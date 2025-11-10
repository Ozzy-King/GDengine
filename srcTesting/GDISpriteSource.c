#include "GDISprite.h"

IWICImagingFactory* pFactory = NULL;
IWICBitmapDecoder* pDecoder = NULL;
IWICBitmapFrameDecode* pFrame = NULL;
IWICFormatConverter* pConverter = NULL;

//itilises the WIC factory for image loading
int GDSPinit(){//add error checking
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(hr != S_OK){ return 1; }
	//crate the WIC imageing factory(ued for later operations)
    hr = CoCreateInstance(
        &CLSID_WICImagingFactory, 
		NULL, 
		CLSCTX_INPROC_SERVER,
		&IID_IWICImagingFactory,
        (void**)&pFactory
    );
	if(hr != S_OK){ return 1; }
	return 0;
}
int GDSPdeinit(){
	if (pConverter) pConverter->lpVtbl->Release(pConverter);
    if (pFrame) pFrame->lpVtbl->Release(pFrame);
    if (pDecoder) pDecoder->lpVtbl->Release(pDecoder);
    if (pFactory) pFactory->lpVtbl->Release(pFactory);
    CoUninitialize();
	return 0;
}

HBITMAP CreateHBITMAPFromWIC(IWICBitmapSource* pBitmapSource)
{
    unsigned int width, height;
	
	pBitmapSource->lpVtbl->GetSize(pBitmapSource, &width, &height);

    // Prepare bitmap info
    BITMAPINFO bminfo = {};
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -(LONG)height; // top-down DIB
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;

	void* pvImageBits = NULL;

    HBITMAP hBmp = CreateDIBSection(_GDconsoleDeviceContext, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    if (!hBmp) return NULL;

    // Copy pixels from WIC to HBITMAP
	const UINT stride = width * 4;
    const UINT imageSize = stride * height;
    HRESULT hr = pBitmapSource->lpVtbl->CopyPixels(pBitmapSource, NULL, stride, imageSize, (BYTE*)pvImageBits);

    if (FAILED(hr))
    {
        DeleteObject(hBmp);
        return NULL;
    }

    return hBmp;
}

//take in the image files name, crates the correct decoders then converts to DISection for further bitblts
//populates the sprite struct with bitmap, image width and height
int _GDSPloadImage(const wchar_t* filename, struct GDSPsprite* spriteOut){
	HBITMAP newBITMAP = NULL;
	// Create decoder from file
    HRESULT hr = pFactory->lpVtbl->CreateDecoderFromFilename(
        pFactory, filename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder
    );
    if(hr != S_OK){ goto cleanUpConverters; }

    // Get the first frame (for most image formats)
    hr = pDecoder->lpVtbl->GetFrame(pDecoder, 0, &pFrame);
    if(hr != S_OK){ goto cleanUpConverters; }

    // Convert to 32bpp BGRA for HBITMAP
    hr = pFactory->lpVtbl->CreateFormatConverter(pFactory, &pConverter);
    if(hr != S_OK){ goto cleanUpConverters; }

    hr = pConverter->lpVtbl->Initialize(//image must be 32bit 8bit for RGBA individually 
        pConverter,
		(IWICBitmapSource*)pFrame,
		&GUID_WICPixelFormat32bppBGRA,
        WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom
    );
    if(hr != S_OK){ goto cleanUpConverters; }
	
	newBITMAP = CreateHBITMAPFromWIC((IWICBitmapSource*)pConverter);
	
	//populate sprite with the bitmap and, width and height
	spriteOut->_spriteMap = newBITMAP;
	((IWICBitmapSource*)pConverter)->lpVtbl->GetSize(((IWICBitmapSource*)pConverter), &spriteOut->_imgWidth, &spriteOut->_imgHeight);

cleanUpConverters:
	if (pConverter){ pConverter->lpVtbl->Release(pConverter); pConverter = NULL;}
	if (pFrame){ pFrame->lpVtbl->Release(pFrame); pFrame = NULL;}
    if (pDecoder){ pDecoder->lpVtbl->Release(pDecoder); pDecoder = NULL;}
	
	return 0;
}


struct GDSPsprite GDSPcreateSprite(wchar_t* fileName, UINT tileWidth, UINT tileHeight, int transparent , COLORREF transparentColour) {
	struct GDSPsprite newSprite;
	unsigned int imgWidth, imgHeight;
	
	//gets sprite into the GDSPsprite, with its width and height
	_GDSPloadImage(fileName, &newSprite);
	imgWidth = newSprite._imgWidth;
	imgHeight = newSprite._imgHeight;
	
	newSprite._tileWidth = tileWidth;
	newSprite._tileHeight = tileHeight;
	newSprite._transparent = transparent;
	newSprite._transparentColour = transparentColour;
	
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