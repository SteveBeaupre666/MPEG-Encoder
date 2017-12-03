#include "Window.h"

CWindow::CWindow()
{
	Initialize();
	hInstance = GetModuleHandle(0);
}

CWindow::~CWindow()
{
	Destroy();
}

void CWindow::Initialize()
{
	hWnd = NULL;
	Fullscreen = false;
	Registered = false;
	ZeroMemory(Caption,   256);
	ZeroMemory(ClassName, 256);
}

HWND CWindow::GetHandle()
{
	return hWnd;
}

bool CWindow::IsFullscreen()
{
	return Fullscreen;
}

char* CWindow::GetCaption()
{
	return Caption;
}

char* CWindow::GetClassName()
{
	return ClassName;
}

void CWindow::SetCaption(char* caption)
{
	ZeroMemory(Caption, 256);
	_snprintf(Caption, 256, "%s", caption);
}

void CWindow::SetClassName(char* class_name)
{
	ZeroMemory(ClassName, 256);
	_snprintf(ClassName, 256, "%s", class_name);
}

int CWindow::CalcWidth(RECT &r)
{
	return r.right - r.left;
}

int CWindow::CalcHeight(RECT &r)
{
	return r.bottom - r.top;
}

void CWindow::SetRect(RECT &r, int l, int t, int w, int h)
{
	r.left   = l;
	r.top    = t;
	r.right  = w;
	r.bottom = h;
}

void CWindow::SetWindowDimention(CWindowDimention &ws, int l, int t, int w, int h)
{
	ws.Left = l;
	ws.Top  = t;
	ws.Width  = w;
	ws.Height = h;
}

int CWindow::GetWindowWidth()
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	return CalcWidth(Rect);
}

int CWindow::GetWindowHeight()
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	return CalcHeight(Rect);
}

int CWindow::GetWidth()
{
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	return CalcWidth(Rect);
}

int CWindow::GetHeight()
{
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	return CalcHeight(Rect);
}

void CWindow::GetSize(int &w, int &h)
{
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	w = CalcWidth(Rect);
	h = CalcHeight(Rect);
}

bool CWindow::Register(WNDPROC MsgProc)
{
	if(Registered)
		return true;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MsgProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = NULL;
	wc.hIconSm       = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = ClassName;

	if(RegisterClassEx(&wc))
		Registered = true;

	return Registered;
}

void CWindow::Unregister()
{
	if(Registered){
		UnregisterClass(ClassName, hInstance);
		Registered = false;
	}
}

bool CWindow::ChangeResolution(int w, int h)
{
	// Get current screen resolution
	DEVMODE dm;
	memset(&dm, 0, sizeof(DEVMODE));
	if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
		return false;

	// Set Display mode to desired resolution
	dm.dmPelsWidth	 = w;
	dm.dmPelsHeight = h;
	//dm.dmBitsPerPel = 32; // Don't change the color depth
			
	// Change the screen resolution (or do nothing if that's the current screen resolution)
	int ChangeDisplayResult = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);	
	if(ChangeDisplayResult != DISP_CHANGE_SUCCESSFUL)
		return false;

	return true;
}

void CWindow::CreateGameWindow(CWindowDimention &ws, DWORD Style)
{
	hWnd = CreateWindowEx(0, 
		ClassName, Caption, Style, 
		ws.Left, ws.Top, ws.Width, ws.Height, 
		0, 0, hInstance, 0);

	DWORD err = GetLastError();
}

void CWindow::ProcessMessages()
{
	if(!hWnd)
		return;

	MSG msg;
	while(PeekMessage(&msg, hWnd, NULL, NULL, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HWND CWindow::Create(bool fs, int w, int h, char *caption, char *class_name)
{
	Fullscreen = fs;
	CWindowDimention ws;

	// Get the screen current resultion size
	int ScrWidth  = GetSystemMetrics(SM_CXSCREEN);
	int ScrHeight = GetSystemMetrics(SM_CYSCREEN);

	// If w or h equal zero or less, set current screen size as default values 
	if(w <= 0 || h <= 0){
		w = ScrWidth;
		h = ScrHeight;
	}

	// Set the window style
	DWORD Style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	if(Fullscreen){
		// Add the fullscreen style
		Style |= WS_POPUP;

		// Try to change the resolution...
		if(!ChangeResolution(w,h))
			return NULL;

		// Set the window dimention to match the screen size
		SetWindowDimention(ws, 0,0,w,h);
	} else {
		// Add the windowed style
		Style |= WS_OVERLAPPEDWINDOW;

		RECT TmpRect;
		SetRect(TmpRect, 0,0,w,h);

		// Adjust the windows size to the desired size
		AdjustWindowRect(&TmpRect, Style, NULL);

		// Compute window dimention
		w = CalcWidth(TmpRect);
		h = CalcHeight(TmpRect);

		// Compute window position
		int l = (ScrWidth  - w) / 2;
		int t = (ScrHeight - h) / 2;

		// Set the window dimention to match the client size
		SetWindowDimention(ws, l,t,w,h);
	}

	// Set window caption
	SetCaption(caption);
	SetClassName(class_name);

	// Register the class
	Register(MsgProc);

	// Try to create the window
	CreateGameWindow(ws, Style);

	if(hWnd){
		// Show the window, redraw it, and set focus on it
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
		SetFocus(hWnd);

		// Clip the cursor to window size if fullscreen
		if(Fullscreen){
			RECT Rect;
			GetWindowRect(hWnd, &Rect);
			ClipCursor(&Rect);
		}
	}

	return hWnd;
}

void CWindow::Destroy()
{
	// Clean up the mess...
	if(hWnd){
		DestroyWindow(hWnd);
		Unregister();
		Initialize();
	}
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;	
	}

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

