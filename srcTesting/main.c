#include "GDIEngine.h"
#include "GDISprite.h"

int main(){
	
	GDinit(640, 360, 1, 1, L"Привет мир");
	//GDsetTitle("testing");
	GDSPinit();
	//load test sprite
	struct GDSPsprite testImg = GDSPcreateSprite(L"./testCard.png", 640, 360, 640, 360, 0, RGB(0,0,0));
	
	
	GDclear(RGB(83,195,189));
	while(_GDwindowOpen){

		if(GDisKeyPressed('A')){
			for (int x = 0; x < _GDwidth; x++)
				for (int y = 0; y < _GDheight; y++)
					GDdrawRawPixel(x, y, RGB(rand()%255, rand()%255, rand()%255));
		}else{
			GDSPdrawSprite(0, 0, 640, 360, &testImg, 0);
		}
		GDdrawBackBuffer();
	}
	
	GDSPdeinit();
	GDdeInit();
	return 0;
}