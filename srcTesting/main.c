#include "GDIEngine.h"

int main(){
	
	GDinit(200, 200, 2, 2, "thing");
	GDclear(RGB(83,195,189));
	
	while(1){
		GDdrawPixel(rand()%200, rand()%200, RGB(rand()%255,rand()%255,rand()%255));
		GDdrawBackBuffer();
	}
	
	return 0;
}