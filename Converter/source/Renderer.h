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
//----------------------------------------------------------------------//
#include "Rectangle.h"
#include "TextureBuffer.h"
#include "ColorConversion.h"
//----------------------------------------------------------------------//
#define BACKGROUND_COLOR 0.10f
//----------------------------------------------------------------------//

#define PRIMARY_THREAD		0
#define SECONDARY_THREAD	1

typedef BOOL (APIENTRY *PFVSYNC)(int);

#define GL_CLEAR_FLAGS	(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)

struct CBackgroundColor {
	float r,g,b,a;
};

class CRenderer {
public:
	CRenderer();
	~CRenderer();
protected:
	HDC   hDC;
	HGLRC hRC[2];
	int   CurrentContext;
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

	CTextureBuffer   TextureBuffer;
	CBackgroundColor BackgroundColor;
private:
	void Reset();
	void ResetTextureData();

	bool CheckExtension(char *extName);
	bool CheckVersion(int MajVer, int MinVer);

	bool SetupPixelFormatDescriptor(HDC hdc);

	void CalcWindowSize();
	
	UINT GetTextureFormat(UINT bpp);
	bool IsTextureFormatValid(UINT bpp);

	void SetClearColor();
	void Set2DMode();
	void DrawQuad();
public:
	//void SetWindow(HWND h);
	//void ReleaseDeviceContext();
	
	bool IsContextValid(int ctx);
	bool SelectContext(int ctx);
	
	void InitOpenGLContext();

	void SetWindow(HWND h);

	bool CreatePrimaryContext();
	void DeletePrimaryContext();
	void CreateSecondaryContext();
	void DeleteSecondaryContext();

	bool CreateTexture(UINT w, UINT h, UINT bpp);
	void UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV);
	void DeleteTexture();

	void SetBackgroundColor(float r, float g, float b, float a = 0.0f);
	
	void Render();
};


