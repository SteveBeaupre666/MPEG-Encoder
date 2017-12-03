#pragma once

#include "Windows.h"
#include "stdio.h"

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct CWindowDimention {
	int Left;
	int Top;
	int Width;
	int Height;
};

class CWindow {
public:
	CWindow();
	~CWindow();
protected:
	HWND hWnd;
	bool Registered;
	bool Fullscreen;
	HINSTANCE hInstance;

	char Caption[256];
	char ClassName[256];
private:
	void Initialize();
	bool ChangeResolution(int w, int h);

	char* GetClassName();
	void  SetClassName(char* class_name);

	int  CalcWidth(RECT &r);
	int  CalcHeight(RECT &r);
	void SetRect(RECT &r, int l, int t, int w, int h);

	bool Register(WNDPROC MsgProc);
	void Unregister();
	
	void SetWindowDimention(CWindowDimention &ws, int l, int t, int w, int h);
	void CreateGameWindow(CWindowDimention &ws, DWORD Style);
public:
	void ProcessMessages();

	HWND Create(bool fs, int w, int h, char *caption, char *class_name);
	void Destroy();

	HWND GetHandle();
	bool IsFullscreen();

	char* GetCaption();
	void  SetCaption(char* caption);

	int  GetWidth();
	int  GetHeight();
	void GetSize(int &w, int &h);

	int  GetWindowWidth();
	int  GetWindowHeight();
};

