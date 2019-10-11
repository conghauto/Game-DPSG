#include "game.h"
#include "dxgraphics.h"
#include <cstdlib>
#include <limits>
#include <float.h>

LPDIRECT3DSURFACE9 back;
LPD3DXSPRITE sprite_handler;
SPRITE rect2;
LPDIRECT3DTEXTURE9 ball_image;
LPDIRECT3DTEXTURE9 paddle_image;
LPDIRECT3DTEXTURE9 paddle_image2;
SPRITE rect1;
SPRITE rectB;
D3DXIMAGE_INFO info;
long start = GetTickCount();
HRESULT result;

SPRITE edgeTop(0, 0, SCREEN_WIDTH, 10);
SPRITE edgeBottom(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10);
SPRITE edgeLeft(0, 0, 0, SCREEN_HEIGHT);
SPRITE edgeRight(SCREEN_WIDTH, 0, 0, SCREEN_HEIGHT);
SPRITE edgeArr[4] = { edgeTop, edgeBottom, edgeLeft, edgeRight };

bool AABBCheck(SPRITE b1, SPRITE b2)
{
	return !(b1.x + b1.width < b2.x || b1.x > b2.x + b2.width || b1.y + b1.height < b2.y || b1.y > b2.y + b2.height);
}

// returns a SPRITE the spans both a current SPRITE and the destination SPRITE
SPRITE GetSweptBroadphaseSprite(SPRITE b)
{
	SPRITE broadphaseSPRITE(0.0f, 0.0f, 0.0f, 0.0f);

	broadphaseSPRITE.x = b.movex > 0 ? b.x : b.x + b.movex;
	broadphaseSPRITE.y = b.movey > 0 ? b.y : b.y + b.movey;
	broadphaseSPRITE.width = b.movex > 0 ? b.movex + b.width : b.width - b.movex;
	broadphaseSPRITE.height = b.movey > 0 ? b.movey + b.height : b.height - b.movey;

	return broadphaseSPRITE;
}

// performs collision detection on moving SPRITE b1 and static SPRITE b2
// returns the time that the collision occured (where 0 is the start of the movement and 1 is the destination)
// getting the new position can be retrieved by SPRITE.x = SPRITE.x + SPRITE.movex * collisiontime
// normalx and normaly return the normal of the collided surface (this can be used to do a response)
float SweptAABB(SPRITE b1, SPRITE b2, float& normalx, float& normaly)
{
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	// find the distance between the objects on the near and far sides for both x and y
	if (b1.movex > 0.0f)
	{
		xInvEntry = b2.x - (b1.x + b1.width);
		xInvExit = (b2.x + b2.width) - b1.x;
	}
	else
	{
		xInvEntry = (b2.x + b2.width) - b1.x;
		xInvExit = b2.x - (b1.x + b1.width);
	}

	if (b1.movey > 0.0f)
	{
		yInvEntry = b2.y - (b1.y + b1.height);
		yInvExit = (b2.y + b2.height) - b1.y;
	}
	else
	{
		yInvEntry = (b2.y + b2.height) - b1.y;
		yInvExit = b2.y - (b1.y + b1.height);
	}

	// find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
	float xEntry, yEntry;
	float xExit, yExit;

	if (b1.movex == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / b1.movex;
		xExit = xInvExit / b1.movex;
	}

	if (b1.movey == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / b1.movey;
		yExit = yInvExit / b1.movey;
	}

	// find the earliest/latest times of collision
	float entryTime = (xEntry > yEntry) ? xEntry : yEntry;
	float exitTime = (xExit > yExit) ? xExit : yExit;

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		return 1.0f;
	}
	else // if there was a collision
	{
		// calculate normal of collided surface
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalx = 1.0f;
				normaly = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
			}
		}
		else
		{
			if (yInvEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
			}
		}

		// return the time of collision
		return entryTime;
	}
}

void MoveBall()
{
	SPRITE broadphasebox = GetSweptBroadphaseSprite(rectB);
	bool isFindSomethingToCollision = false;

	//xet va cham voi man hinh
	
	if (rectB.x > SCREEN_WIDTH || rectB.x < 0)
	{
		MessageBox(NULL, "Game Over",
			"Account Details", MB_OK);
		exit(0);
	}
	if (rectB.y > SCREEN_HEIGHT - rectB.height-10 || rectB.y - rectB.width < 0)
		rectB.movey *= -1;
	// xet xem co kha nang va cham voi spritesurface1 khong
	if (AABBCheck(broadphasebox, rect1))
	{
		isFindSomethingToCollision = true;

		float normalx, normaly;

		float collisiontime = SweptAABB(rectB, rect1, normalx, normaly);
		rectB.x += rectB.movex * collisiontime;
		rectB.y += rectB.movey * collisiontime;

		float remainingtime = 1.0f - collisiontime;

		if (collisiontime != 1.0f)
		{
			//phan xa
			remainingtime = 1.0f;  // tuc la da giam 1.0 luc so voi ban dau
			rectB.movex *= remainingtime;
			rectB.movey *= remainingtime;
			if (std::abs(normalx) > 0.0001f)
				rectB.movex = -rectB.movex;
			if (std::abs(normaly) > 0.0001f)
				rectB.movey = -rectB.movey;
		}
	}

	// xet xem co kha nang va cham voi spritesurface2 khong
	if (AABBCheck(broadphasebox, rect2))
	{
		isFindSomethingToCollision = true;

		float normalx, normaly;

		float collisiontime = SweptAABB(rectB, rect2, normalx, normaly);
		rectB.x += rectB.movex * collisiontime;
		rectB.y += rectB.movey * collisiontime;

		float remainingtime = 1.0f - collisiontime;

		if (collisiontime != 1.0f)
		{
			//phan xa
			remainingtime = 1.0f;  // tuc la da giam 0.0 luc so voi ban dau
			rectB.movex *= remainingtime;
			rectB.movey *= remainingtime;
			if (std::abs(normalx) > 0.0001f)
				rectB.movex = -rectB.movex;
			if (std::abs(normaly) > 0.0001f)
				rectB.movey = -rectB.movey;
		}
	}

	// neu khong phat hien ra cai nao co the gay ra truong hop va cham thi cu di chuyen binh thuong
	if (isFindSomethingToCollision == false) {
		rectB.x += rectB.movex;
		rectB.y += rectB.movey;
	}
}
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
	char b[20] = "ball.png";
	ball_image = LoadTexture(b, D3DCOLOR_XRGB(255, 10, 255));
	if (ball_image == NULL)
	{
		return 0;
	}
	rectB.x = SCREEN_WIDTH/2;
	rectB.y = 100;
	rectB.width = 30;
	rectB.height = 30;
	rectB.movex = 2;
	rectB.movey = 2;
	char a[20] = "char1.jpg";
	paddle_image = LoadTexture(a, D3DCOLOR_XRGB(255, 0, 255));
	if (paddle_image == NULL)
		return 0;
	//set paddle properties
	rect1.x = 445;
	rect1.y = 0;
	rect1.width = 20;
	rect1.height = 110;
	//return okay
	
	paddle_image2 = LoadTexture(a, D3DCOLOR_XRGB(255, 10, 255));
	if (paddle_image2 == NULL)
		return 0;
	//set paddle properties
	rect2.x = 0;
	rect2.y = 0;
	rect2.width = 20;
	rect2.height = 110;

	return 1;
}
//the main game loop
void Game_Run(HWND hwnd)
{
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));
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
		rectB.x += rectB.movex;
		rectB.y += rectB.movey;
		//move the paddle
		rect1.y  += Mouse_Y();
		if (rect1.y > SCREEN_HEIGHT - rect1.height)
			rect1.y = SCREEN_HEIGHT - rect1.height;
		else if (rect1.y < 0)
			rect1.y = 0;
		//check for left arrow
		if (Key_Down(DIK_UP))
			rect2.y -= 7;
		//check for right arrow
		if (Key_Down(DIK_DOWN))
			rect2.y += 7;
		if (rect2.y > SCREEN_HEIGHT - rect2.height)
			rect2.y = SCREEN_HEIGHT - rect2.height;
		else if (rect2.y < 0)
			rect2.y = 0;
		MoveBall();
	}
	//start rendering
	if (d3ddev->BeginScene())
	{
		//erase the entire background
		d3ddev->StretchRect(back, NULL, backbuffer, NULL, D3DTEXF_NONE);
		//start sprite handler
		sprite_handler->Begin(D3DXSPRITE_ALPHABLEND);
		//draw the paddle
		position.x = (float)rectB.x;
		position.y = (float)rectB.y;
		sprite_handler->Draw(
			ball_image,
			NULL,
			NULL,
			&position,
			D3DCOLOR_XRGB(255, 255, 255));

		position.x = (float)rect1.x;
		position.y = (float)rect1.y;
		sprite_handler->Draw(
			paddle_image,
			NULL,
			NULL,
			&position,
			D3DCOLOR_XRGB(255, 255, 255));

		position.x = (float)rect2.x;
		position.y = (float)rect2.y;
		sprite_handler->Draw(
			paddle_image2,
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