#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <windows.h>
#include "dxinput.h"
#include "game.h"
#include "dxgraphics.h"

#define APPTITLE "Ping Pong"
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 500

LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE);
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		//release input objects
		Kill_Keyboard();
		Kill_Mouse();
		if (dinput != NULL) dinput->Release();
		Game_End(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APPTITLE;
	wc.hIconSm = NULL;
	return RegisterClassEx(&wc);
}
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	
	MSG msg;
	MyRegisterClass(hInstance);
	HWND hWnd;
	//create a new window
	hWnd = CreateWindow(
		APPTITLE, //window class
		APPTITLE, //title bar
		WS_OVERLAPPEDWINDOW, //window style
		CW_USEDEFAULT, //x position of window
		CW_USEDEFAULT, //y position of window
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		NULL, //parent window
		NULL, //menu
		hInstance, //application instance
		NULL); //window parameters
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
		//was there an error creating the window?
	//initialize DirectInput
	if (!Init_DirectInput(hWnd))
	{
		MessageBox(hWnd, "Error initializing DirectInput", "Error", MB_OK);
		return 0;
	}
	if (!hWnd)
		return FALSE;
	//display the window
	
	//initialize the game
	Init_Direct3D(hWnd);
	if (!Game_Init(hWnd))
		return 0;
	// main message loop
	int done = 0;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//look for quit message
			if (msg.message == WM_QUIT)
				done = 1;
			//decode and pass messages on to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			//process game loop (else prevents running after window is closed)
			Game_Run(hWnd);
	}
	return msg.wParam;
}
