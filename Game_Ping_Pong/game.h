#ifndef _GAME_H
#define _GAME_H 1
//windows/directx headers
#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <dinput.h>
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
//framework-specific headers
#include "dxgraphics.h"
#include "dxinput.h"
//application title
#define APPTITLE "Paddle_Game"
//screen setup
#define FULLSCREEN 0 //0 = windowed, 1 = fullscreen
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 500
//function prototypes
int Game_Init(HWND);
void Game_Run(HWND);
void Game_End(HWND);
//sprite structure
typedef struct SPRITE {
	SPRITE() {}
	SPRITE(int _x, int _y, float _width, float _height, float _movex, float _movey) {
		x = _x;
		y = _y;
		width = _width;
		height = _height;
		movex = _movex;
		movey = _movey;
	}

	SPRITE(int _x, int _y, float _width, float _height) {
		x = _x;
		y = _y;
		width = _width;
		height = _height;
		movex = 0.0f;
		movey = 0.0f;
	}
	int x, y;				// vi tri top (~x) va left (~y)
	float width, height;	//chieu dai + rong
	float movex, movey;		// van toc doi tuong
	int curframe, lastframe;
	int animdelay, animcount;
	int scalex, scaley;
	int rotation, rotaterate;
} ;
#endif