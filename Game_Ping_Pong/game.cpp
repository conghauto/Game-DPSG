#include "game.h"
#include "dxgraphics.h"
LPDIRECT3DSURFACE9 back;
LPD3DXSPRITE sprite_handler;
SPRITE rect2;
LPDIRECT3DTEXTURE9 paddle_image;
LPDIRECT3DTEXTURE9 paddle_image2;
SPRITE ball;

D3DXIMAGE_INFO info;
long start = GetTickCount();
HRESULT result;
int Game_Init(HWND hwnd)
{
	//initialize mouse
	if (!Init_Mouse(hwnd))
	{
		MessageBox(hwnd, "Error initializing the mouse", "Error", MB_OK);
		return 0;
	}
	////initialize keyboard
	if (!Init_Keyboard(hwnd))
	{
		MessageBox(hwnd, "Error initializing the keyboard", "Error", MB_OK);
		return 0;
	}
	//create sprite handler object
	result = D3DXCreateSprite(d3ddev, &sprite_handler);
	if (result != D3D_OK)
		return 0;
	//load the background image
	
	//load the paddle sprite
	char a[20] = "char1.jpg";
	paddle_image = LoadTexture(a, D3DCOLOR_XRGB(255, 0, 255));
	if (paddle_image == NULL)
		return 0;
	//set paddle properties
	ball.x = 0;
	ball.y = 0;
	ball.width = 32;
	ball.height = 32;
	ball.movex = 10;
	ball.movey = 10;
	//return okay
	
	return 1;
}
//the main game loop
void Game_Run(HWND hwnd)
{
	//ball position vector
	D3DXVECTOR3 position(0, 0, 0);
	//make sure the Direct3D device is valid
	if (d3ddev == NULL)
		return;
	//update mouse and keyboard
	Poll_Mouse();
	Poll_Keyboard();
	//after short delay, ready for next frame?
	//this keeps the game running at a steady frame rate
	if (GetTickCount() - start >= 30)
	{
		//reset timing
		start = GetTickCount();
		
		//move the paddle
		
		ball.x += ball.movex;
		
		//ball.y += ball.movey;
		//bounce the ball at screen edges
		if (ball.x > SCREEN_WIDTH - ball.width)
		{
			ball.x = SCREEN_WIDTH - ball.width;
			ball.y += ball.movey;
			
		}
		else if (ball.x < 0)
		{
			ball.x = 0;
			ball.y -= ball.movey;
			
		}
		if (ball.y > SCREEN_HEIGHT - ball.height)
		{
			ball.y = 426;
			ball.movex *=-1;
			
		}
		else if (ball.y < 0)
		{
			ball.y = 0;
			ball.movex *= -1;
			
		}
		
		//check for left arrow

	}
	//start rendering
	if (d3ddev->BeginScene())
	{
		d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));
		//erase the entire background
		d3ddev->StretchRect(back, NULL, backbuffer, NULL, D3DTEXF_NONE);
		//start sprite handler
		sprite_handler->Begin(D3DXSPRITE_ALPHABLEND);
		//draw the paddle
		position.x = (float)ball.x;
		position.y = (float)ball.y;
		sprite_handler->Draw(
			paddle_image,
			NULL,
			NULL,
			&position,
			D3DCOLOR_XRGB(255, 255, 255));
		//stop drawing
		sprite_handler->End();
		//stop rendering
		d3ddev->EndScene();
	}
	//display the back buffer on the screen
	d3ddev->Present(NULL, NULL, NULL, NULL);
	//check for mouse button (to exit program)
	if (Mouse_Button(0))
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	//check for escape key (to exit program)
	if (Key_Down(DIK_ESCAPE))
		PostMessage(hwnd, WM_DESTROY, 0, 0);
}
//frees memory and cleans up before the game ends
void Game_End(HWND hwnd)
{
	if (paddle_image != NULL)
		paddle_image->Release();
	if (back != NULL)
		back->Release();
	if (sprite_handler != NULL)
		sprite_handler->Release();
}