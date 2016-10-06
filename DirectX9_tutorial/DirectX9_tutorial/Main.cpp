// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include "DirectX9/Include/d3d9.h"
//#include "DirectX9/Include/d3dx9.h"

// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

// include the Direct3D Library files
#pragma comment (lib, "DirectX9/Lib/x86/d3d9.lib")
//#pragma comment (lib, "DirectX9/Lib/x86/d3dx9.lib")

//global declarations
LPDIRECT3D9 d3d; // the pointer to out Direct3D interface
LPDIRECT3DDEVICE9 d3ddev; // the pointer to the device class

//function prototypes
void initD3D(HWND hwnd); //initializes direct 3D
void render(void); //renders actual frame
void cleanD3D(void); //closes and releases direct 3D

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//sOLVED TO USE MAIN INSTEad of WinMain
int main(int nCmdShow)
{
	//Creating window
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL,
		"WindowClass",
		"Our First Direct3D Program",
		WS_OVERLAPPEDWINDOW,	//Top window and without borders
		300, 300,						 //Starting position of the window							
		SCREEN_WIDTH, SCREEN_HEIGHT, //Resolution
		NULL,
		NULL,
		GetModuleHandle(0), //substitute of hInstance
		NULL);

	ShowWindow(hWnd, nCmdShow);
	//

	// set up and initialize Direct3D
	initD3D(hWnd);

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		render();
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}

//Wtf
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
//

void initD3D(HWND hwnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp; //struct that holds device info

	ZeroMemory(&d3dpp, sizeof(d3dpp)); //Clears d3dpp faster
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // DOUBLE BUFFERINg yay
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //back buffer format at 32-bit, no alpha channel
	d3dpp.BackBufferWidth = SCREEN_WIDTH;	//BUFFER WIDTH
	d3dpp.BackBufferHeight = SCREEN_HEIGHT; //buffer height

	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, //Using software for vertex processing for now
		&d3dpp,
		&d3ddev);
}

void cleanD3D(void)
{
	d3ddev->Release();
	d3d->Release();
}



void render(void)
{

	

	//Clears the window to a default state
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 40, 100), 1.0f, 0);

	d3ddev->BeginScene();

	// Here goes the 3d magic

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}