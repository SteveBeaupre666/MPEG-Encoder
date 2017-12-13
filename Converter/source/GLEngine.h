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
#include "TextureBuffer.h"
#include "ColorConversion.h"
//----------------------------------------------------------------------//

typedef BOOL (APIENTRY *PFVSYNC)(int);

#define GL_CLEAR_FLAGS	(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

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

	BYTE *buf;
	UINT BufBPP;
	UINT BufWidth;
	UINT BufHeight;

	CTextureBuffer TextureBuffer;
private:
	void Reset();
	void ResetTextureData();
	bool IsOpenGLInitialized;
	bool SetupPixelFormatDescriptor(HDC hdc);

	void CalcWindowSize();
	
	UINT GetTextureFormat(UINT bpp);
	bool IsTextureFormatValid(UINT bpp);

	void Set2DMode();
	void DrawQuad();
public:
	bool IsInitialized();
	bool Initialize(HWND hwnd);
	void Shutdown();

	bool CreateTexture(UINT w, UINT h, UINT bpp);
	void UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV);
	void DeleteTexture();

	void Render();
};


