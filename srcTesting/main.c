#include "GDIEngine.h"

int main(){
	
	GDinit(200, 200, 4, 4, "thing");
	GDclear(RGB(83,195,189));
	
	
	while(_GDwindowOpen){
		if(GDisKeyPressed('A')){
			GDclear(RGB(0,0,0));
			//printf("a");
		}else{
			GDclear(RGB(83,195,189));
		}
		GDdrawBackBuffer();
	}
	GDdeInit();
	return 0;
}