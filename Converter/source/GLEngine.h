#pragma once
//----------------------------------------------------------------------//
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")	 
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include "GLExtentions.h"
//----------------------------------------------------------------------//
#include "Rectangle.h"
#include "ColorConversion.h"
//----------------------------------------------------------------------//

#define USE_NEW_CODE

typedef BOOL (APIENTRY *PFVSYNC)(int);

class CGLEngine {
public:
	CGLEngine();
	~CGLEngine();
protected:
	HDC   hDC;
	HGLRC hRC;
protected:
	PFVSYNC VSync;
private:
	HWND hWnd;
	UINT WndWidth;
	UINT WndHeight;

	UINT TexID;
	UINT TexWidth;
	UINT TexHeight;
	UINT TexFormat;

	BYTE *Buffer;
	UINT BufferBPP;
	UINT BufferSize;
	UINT BufferWidth;
	UINT BufferHeight;
private:
	void Reset();
	bool IsOpenGLInitialized;
	bool SetupPixelFormatDescriptor(HDC hdc);

	void Set2DMode();
	void CalcWindowSize();
	void DrawQuad();

	UINT GetNextPowerOfTwo(UINT n);
	void CreateTextureBuffer(UINT w, UINT h);
	void EraseTextureBuffer();
	void DeleteTextureBuffer();
public:
	bool IsInitialized();
	bool Initialize(HWND hwnd, bool VSync = true);
	void Shutdown();

	void CreateTexture(UINT w, UINT h, UINT bpp);
	void UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV);
	void DeleteTexture();

	void Render();
};


