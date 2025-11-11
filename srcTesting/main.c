#include "GDIEngine.h"
#include "GDISprite.h"
#include <time.h>

const int paddleWidth =10;
const int paddleHeight =70;
const int ballWidth =20;
const int ballHeight =20;

const int paddleOffset =5;

int player1Y = 0, player2Y = 0;
float ballVecX;
float ballVecY;
float ballPosX;
float ballPosY;
void updateBallPos(){
	ballPosX += ballVecX;
	ballPosY += ballVecY;
	if (ballPosY < 0){ballPosY = 0; ballVecY = -ballVecY;}
	if (ballPosY > _GDheight - ballHeight){ballPosY = _GDheight - ballHeight; ballVecY = -ballVecY;}
	
	if(ballPosY > player1Y-ballHeight && ballPosY < player1Y + paddleHeight 
		&& ballPosX < paddleOffset + paddleWidth)
	{ballPosX = paddleOffset + paddleWidth; ballVecX = -ballVecX;}	
	
	if(ballPosY > player2Y- ballHeight && ballPosY < player2Y + paddleHeight 
		&& ballPosX > _GDwidth-paddleOffset-paddleWidth - ballWidth)
	{ballPosX = _GDwidth-paddleOffset-paddleWidth-ballWidth; ballVecX = -ballVecX;}	

	if (ballPosX < 0 || ballPosX > _GDwidth-ballWidth){ 
		ballPosX = _GDwidth/2;
		ballPosY = _GDheight/2;
		ballVecX = ((float)rand() / RAND_MAX) * 0.6 - 0.3;
		ballVecY = ((float)rand() / RAND_MAX) * 0.6 - 0.3;
	}
}

int main(){
	
	GDinit(640, 360, 1, 1, L"Привет мир");
	//GDsetTitle("testing");
	GDSPinit();
	srand(time(NULL));
	ballPosX = _GDwidth/2;
	ballPosY = _GDheight/2;
	ballVecX = ((float)rand() / RAND_MAX) * 0.6 - 0.3;
	ballVecY = ((float)rand() / RAND_MAX) * 0.6 - 0.3;
	
	//load test sprite
	struct GDSPsprite paddleGFX = GDSPcreateSprite(L"./paddle.jpg", paddleWidth, paddleHeight, 0, RGB(0,0,0));
	struct GDSPsprite ballGFX = GDSPcreateSprite(L"./ball.jpg", ballWidth, ballHeight, 0, RGB(0,0,0));
	
	//GDclear(RGB(83,195,189));
	while(_GDwindowOpen){
		GDclearBlack();
		if(GDisKeyPressed('W') && player1Y != 0){
			player1Y--;
		}
		else if (GDisKeyPressed('S') && player1Y != _GDheight - paddleHeight){
			player1Y++;
		}
		if (GDisKeyPressed(VK_UP)&& player2Y != 0){
			player2Y--;
		}
		else if (GDisKeyPressed(VK_DOWN)&& player2Y != _GDheight - paddleHeight){
			player2Y++;
		}
		//draw player 1, draw player 2
		GDSPdrawSprite(paddleOffset, player1Y, paddleWidth, paddleHeight, &paddleGFX, 0);
		GDSPdrawSprite(_GDwidth-paddleWidth -paddleOffset, player2Y, paddleWidth, paddleHeight, &paddleGFX, 0);
		
		updateBallPos();
		GDSPdrawSprite(ballPosX, ballPosY, ballWidth, ballHeight, &ballGFX, 0);
		
		
		GDdrawBackBuffer();
	}
	
	GDSPdeinit();
	GDdeInit();
	return 0;
}