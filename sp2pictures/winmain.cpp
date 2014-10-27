// Beginning Game Programming, Second Edition
// Chapter 6 - load bitmaps and create surfaces
// Pictures
// This program demonstrates how to load and display a picture with DirectX
// The demonstrated functionality is performed by the D3DX helper library. 
// To enable it's use add the d3dx9.lib to the additional dependencies in 
// the project properties linker tab.
// 
// Supported formats are .bmp, .dds, .dib, .jpg, .png, .tga

//header files to include
#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
//application title
#define APPTITLE "Load_Buttmap"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
//macros to read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//forward declarations
LRESULT WINAPI WinProc(HWND, UINT, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE);
int Game_Init(HWND);
void Game_Run(HWND);
void Game_End(HWND);

//Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;

//Buffers
LPDIRECT3DSURFACE9 backBuffer = NULL;//Backbuffer is where we paint before displaying
LPDIRECT3DSURFACE9 surface = NULL;//Surface is what is displayed

//window event callback function
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Game_End(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//helper function to set up the window properties
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	//create the window class structure
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	//fill the struct with info
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APPTITLE;
	wc.hIconSm = NULL;
	//set up the window with the class info
	return RegisterClassEx(&wc);
}
//entry point for a Windows program
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
	)
{
	// declare variables
	MSG msg;
	// register the class
	MyRegisterClass(hInstance);

	// initialize application
	//note––got rid of initinstance
	HWND hWnd;
	//create a new window
	hWnd = CreateWindow(
		APPTITLE, //window class
		APPTITLE, //title bar
		WS_OVERLAPPEDWINDOW,//WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP, window style
		CW_USEDEFAULT, //x position of window
		CW_USEDEFAULT, //y position of window
		SCREEN_WIDTH, //width of the window
		SCREEN_HEIGHT, //height of the window
		NULL, //parent window
		NULL, //menu
		hInstance, //application instance
		NULL
		); //window parameters

	//was there an error creating the window?
	if (!hWnd)
		return FALSE;
	//display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//initialize the game
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
			{
				MessageBox(hWnd, "Received WM_QUIT message", "WinMain", MB_OK);
				done = 1;
			}
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


int Game_Init(HWND hwnd){
	HRESULT result;

	//initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		MessageBox(hwnd, "Error initializing Direct3D", "Error", MB_OK);
		return 0;
	}

	//set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;//struct for setting up
	ZeroMemory(&d3dpp, sizeof(d3dpp));//clear out struct

	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.hDeviceWindow = hwnd;

	//create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,//use default video card
		D3DDEVTYPE_HAL,// use hardware renderer
		hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,//do not use T&L
		&d3dpp,// presentation parameters
		&d3ddev);

	if (d3ddev == NULL)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return 0;
	}

	//set random number seed
	srand(time(NULL));

	//clear backbuffer and create a pointer to the backbuffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	//create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		640,//width
		480,//height
		D3DFMT_X8R8G8B8,//surface format
		D3DPOOL_DEFAULT,//memorypool to use
		&surface,//pointer to the surface
		NULL);

	if (result!=D3D_OK)
		return 1;

	//load surface from file
	result = D3DXLoadSurfaceFromFile(
		surface,//destination surface
		NULL,//destination palette
		NULL,//destination rectangle
		"moi.png",//source filename
		NULL,//source rectangle
		D3DX_DEFAULT,//controls how image is filtered
		0,//for transparency (0 for none)
		NULL);//source image infor (usually null)

	//make sure file was loaded okay
	if (result != D3D_OK)
		return 1;

	//draw surface to the backbuffer
	d3ddev->StretchRect(surface, NULL, backBuffer, NULL, D3DTEXF_NONE);

	return 1;
}

void Game_Run(HWND hwnd)
{
	//make sure the Direct3D device is valid
	if (d3ddev == NULL)
		return;

	//start rendering
	if (d3ddev->BeginScene())
	{
		//create pointer to the backbuffer
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

		//draw surface to the backbuffer
		d3ddev->StretchRect(surface, NULL, backBuffer, NULL, D3DTEXF_NONE);
		//stop rendering
		d3ddev->EndScene();
	}

	//display the back buffer on the screen
	d3ddev->Present(NULL, NULL, NULL, NULL);

	//check for escape key (to exit program)
	if (KEY_DOWN(VK_ESCAPE))
		PostMessage(hwnd, WM_DESTROY, 0, 0);
}

void Game_End(HWND hwnd)
{
	//free the surface
	surface->Release();

	//release the Direct3D device
	if (d3ddev != NULL)
		d3ddev->Release();

	//release the Direct3D object
	if (d3d != NULL)
		d3d->Release();
}