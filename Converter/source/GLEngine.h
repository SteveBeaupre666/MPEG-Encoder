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
#include "ColorConversion.h"
//----------------------------------------------------------------------//

struct OpenGLVersion {
	int Major;
	int Minor;
};

enum TextureFormat {tfGrayscale, tfRGB, tfRGBA};

typedef BOOL (APIENTRY *PFVSYNC)(int);

class CGLEngine {
public:
	CGLEngine();
	~CGLEngine();
private:
	PFVSYNC VSync;

	void Swap(float &a, float &b);
	int  CharToInt(char c);
	UINT GetNextPowerOfTwo(UINT n);
private:
	HDC   hDC;
	HGLRC hRC;
	HWND  hWnd;

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

	bool CheckExtension(char *extName);
	bool CheckVersion(OpenGLVersion version_number);

	bool SetupPixelFormatDescriptor(HDC hdc);

	void CreateTextureBuffer(UINT w, UINT h);
	void EraseTextureBuffer();
	void DeleteTextureBuffer();

	void CalcWindowSize();
	void Set2DMode();
	void DrawQuad(UINT TextureID);
public:
	BYTE* GetBuffer(){return Buffer;}
	UINT  GetBufferBPP(){return BufferBPP;}
	UINT  GetBufferWidth(){return BufferWidth;}
	UINT  GetBufferHeight(){return BufferHeight;}
	UINT  GetTextureWidth(){return TexWidth;}
	UINT  GetTextureHeight(){return TexHeight;}
	UINT  GetTextureFormat(){return TexFormat;}
public:
	bool IsEngineInitialized();
	bool Initialize(HWND hwnd, bool VSync = true);
	void Shutdown();
	
	void CreateTexture(UINT w, UINT h, TextureFormat fmt = tfRGBA);
	void UpdateTexture(BYTE *pY, BYTE *pU, BYTE *pV);
	void DeleteTexture();

	void Render(BYTE *pY, BYTE *pU, BYTE *pV);
};


