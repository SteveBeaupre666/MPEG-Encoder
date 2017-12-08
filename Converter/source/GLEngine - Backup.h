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
#include "Texture.h"
#include "ColorConversion.h"
//----------------------------------------------------------------------//

//#define NEW_RENDER_CODE

struct OpenGLVersion {
	int Major;
	int Minor;
};

struct GLRect {float l,t,r,b;};

struct GLQuad {
	GLRect v,t;
};

#define GLVERTEX(u, v, x, y) glTexCoord2f((u), (v)); glVertex2f((x), (y))
#define CHECK_ENGINE() if(!IsEngineInitialized())return
typedef BOOL (APIENTRY *PFVSYNC)(int);

class CGLEngine {
public:
	CGLEngine();
	~CGLEngine();
private:
	PFVSYNC VSync;
private:
	HDC   hDC;
	HGLRC hRC;

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

	void CalcWindowSize();

	void CreateTexture(UINT w, UINT h);
	void UpdateTexture(BYTE *y, BYTE *u, BYTE *v);
	void DeleteTexture();

	void Set2DMode();
	void SetViewport(int l, int t, int w, int h);
	void SetViewMatrix(float l, float t, float w, float h);

	void DrawQuad();
public:
	BYTE* GetFrameBuffer(){return Buffer;}
	UINT  GetFrameBufferBPP(){return BufferBPP;}
	UINT  GetFrameBufferWidth(){return BufferWidth;}
	UINT  GetFrameBufferHeight(){return BufferHeight;}

	UINT  GetTextureWidth(){return TexWidth;}
	UINT  GetTextureHeight(){return TexHeight;}
	UINT  GetTextureFormat(){return TexFormat;}
public:
	bool IsEngineInitialized();
	bool Initialize(HWND hwnd, bool VSync = true);
	void Shutdown();
	
	bool CreateFrameBuffer(UINT w, UINT h, UINT bpp);
	void UpdateFrameBuffer(BYTE *pY, BYTE *pU, BYTE *pV);
	void RenderFrameBuffer(BYTE *pY, BYTE *pU, BYTE *pV);
	void DeleteFrameBuffer();
};


