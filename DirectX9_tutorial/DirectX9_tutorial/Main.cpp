// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include "DirectX9/Include/d3d9.h"
#include "DirectX9/Include/d3dx9.h"

// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

// include the Direct3D Library files
#pragma comment (lib, "DirectX9/Lib/x64/d3d9.lib")
#pragma comment (lib, "DirectX9/Lib/x64/d3dx9.lib")

//global declarations
LPDIRECT3D9 d3d; // the pointer to out Direct3D interface
LPDIRECT3DDEVICE9 d3ddev; // the pointer to the device class
LPDIRECT3DVERTEXBUFFER9 v_buffer = NULL;    // the pointer to the vertex buffer
LPDIRECT3DINDEXBUFFER9 i_buffer = NULL;


//function prototypes
void initD3D(HWND hwnd); //initializes direct 3D
void render_frame(void); //renders actual frame
void cleanD3D(void); //closes and releases direct 3D
void init_graphics(); //inittialices the graphics
void init_light();

struct CUSTOMVERTEX { FLOAT X, Y, Z; D3DVECTOR NORMAL; };
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL)

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
		300, 100,						 //Starting position of the window							
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

		render_frame();
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

// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	init_graphics();    // call the function to initialize the triangle
	init_light();

	d3ddev->SetRenderState(D3DRS_LIGHTING, TRUE);    // turn off the 3D lighting
	d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);    // both sides of the triangles
	d3ddev->SetRenderState(D3DRS_ZENABLE, TRUE);    // turn on the z-buffer
	d3ddev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(50, 50, 50)); //sets the ambient light
}


// this is the function used to render a single frame
void render_frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	// select which vertex format we are using
	d3ddev->SetFVF(CUSTOMFVF);

	// SET UP THE PIPELINE
	//transform
	D3DXMATRIX matRotateY;
	D3DXMATRIX matRotateX;
	

	static float index = 0.0f; index += 0.05f;

	D3DXMatrixRotationY(&matRotateY, index);
	D3DXMatrixRotationX(&matRotateX, index * 2);

	d3ddev->SetTransform(D3DTS_WORLD, &(matRotateY));
	// tell Direct3D about our matrix
	

	D3DXMATRIX matView;    // the view transform matrix

	D3DXMatrixLookAtLH(&matView,
		&D3DXVECTOR3(0.0f, 0.0f, 10.0f),    // the camera position
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f),    // the look-at position
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));    // the up direction

	d3ddev->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView

	D3DXMATRIX matProjection;     // the projection transform matrix

	D3DXMatrixPerspectiveFovLH(&matProjection,
		D3DXToRadian(45),    // the horizontal field of view
		(FLOAT)SCREEN_WIDTH / (FLOAT)SCREEN_HEIGHT, // aspect ratio
		1.0f,    // the near view-plane
		100.0f);    // the far view-plane

	d3ddev->SetTransform(D3DTS_PROJECTION, &matProjection);    // set the projection

	// select the vertex buffer to display
	d3ddev->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
	d3ddev->SetIndices(i_buffer);
	
	
	//d3ddev->SetTransform(D3DTS_WORLD, &(matTranslateA * matRotateY));
	d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 24, 0, 12);

	
	//

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	i_buffer->Release();	// close and release the vertex buffer
	v_buffer->Release();    // close and release the vertex buffer
	d3ddev->Release();    // close and release the 3D device
	d3d->Release();    // close and release Direct3D
}


// this is the function that puts the 3D models into video RAM
void init_graphics(void)
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
		//Cube
		// Side 1
		{-2.0f, 2.0f, 2.0f, 0.0f,0.0f, 1.0f}, //0
		{2.0f, 2.0f, 2.0f, 0.0f, 0.0f, 1.0f}, //1
		{-2.0f,-2.0f, 2.0f, 0.0f, 0.0f, 1.0f},//2
		{2.0f, -2.0f, 2.0f, 0.0f, 0.0f, 1.0f},//3
		
		// Side 2
		{-2.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f},//4
		{ -2.0f, 2.0f, 2.0f, -1.0f, 0.0f, 0.0f },//5
		{-2.0f, -2.0f, -2.0f, -1.0f, 0.0f, 0.0f},//6
		{-2.0f, -2.0f, 2.0f, -1.0f, 0.0f, 0.0f},//7

		// Side 3
		{ -2.0f, -2.0f, 2.0f, 0.0f, -1.0f, 0.0f },//8
		{ 2.0f, -2.0f, 2.0f, 0.0f, -1.0f, 0.0f},//9
		{ -2.0f, -2.0f, -2.0f, 0.0f, -1.0f, 0.0f},//10
		{2.0f, -2.0f, -2.0f, 0.0f, -1.0f, 0.0f},//11

		//Side 4
		{2.0f, 2.0f, -2.0f, 0.0f, 0.0f, -1.0f},//12
		{-2.0f, 2.0f, -2.0f, 0.0f, 0.0f, -1.0f},//13
		{2.0f, -2.0f, -2.0f, 0.0f, 0.0f -1.0f},//14
		{-2.0f, -2.0f, -2.0f, 0.0f, 0.0f, -1.0f},//15
		//Side 5
		{2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f},//16
		{2.0f, 2.0f, -2.0f, 1.0f, 0.0f, 0.0f},//17
		{2.0f, -2.0f, 2.0f, 1.0f, 0.0f, 0.0f},//18
		{2.0f, -2.0f, -2.0f, 1.0f, 0.0f, 0.0f},//19

		//Side 6
		{-2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f},//20
		{2.0f, 2.0f, -2.0f, 0.0f, 1.0f, 0.0f},//21
		{-2.0f, 2.0f, 2.0f, 0.0f, 1.0f, 0.0f},//22
		{2.0f, 2.0f, 2.0f, 0.0f, 1.0f, 0.0f},//23



	


		//Pyramid
		/*{0.0f, 2.0f, 0.0f, D3DCOLOR_XRGB(0,0,255),}, //0
		{-2.0f, -2.0f, -2.0f, D3DCOLOR_XRGB(0,255,0),}, //1
		{2.0f, -2.0f, -2.0f, D3DCOLOR_XRGB(255,0,0),}, //2
		{-2.0f, -2.0f, 2.0f, D3DCOLOR_XRGB(0,255,255),}, //3
		{2.0f, -2.0f, 2.0f, D3DCOLOR_XRGB(255,255,0),}, //4*/
	};

	// create a vertex buffer interface called v_buffer
	d3ddev->CreateVertexBuffer(24 * sizeof(CUSTOMVERTEX),
		0,
		CUSTOMFVF,
		D3DPOOL_MANAGED,
		&v_buffer,
		NULL);

	VOID* pVoid;    // a void pointer

					// lock v_buffer and load the vertices into it
	v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	v_buffer->Unlock();

	//indexes
	short indices[]
	{
		//Cube
		0,1,3,
		0,2,3,

		4,5,7,
		4,6,7,

		8,9,11,
		8,10,11,

		12,14,15,
		12,13,15,

		16,18,19,
		16,17,19,

		20,22,23,
		20,21,23,
		
	};

	d3ddev->CreateIndexBuffer(
		36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&i_buffer,
		NULL);

	// lock i_buffer and load the indices into it
	i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	i_buffer->Unlock();
}


void init_light(void)
{
	D3DLIGHT9 light;    // create the light struct
	D3DMATERIAL9 material; //create the material struct

	ZeroMemory(&light, sizeof(light));    // clear out the light struct for use
	light.Type = D3DLIGHT_DIRECTIONAL;    // make the light type 'directional light'
	light.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);    // set the light's color
	light.Direction = D3DXVECTOR3(-1.0f, -0.3f, -1.0f);

	d3ddev->SetLight(0, &light);    // send the light struct properties to light #0
	d3ddev->LightEnable(0, TRUE);    // turn on light #0

	ZeroMemory(&material, sizeof(D3DMATERIAL9));    // clear out the struct for use
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set diffuse color to white
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);    // set ambient color to white

	d3ddev->SetMaterial(&material);    // set the globably-used material to &material
}