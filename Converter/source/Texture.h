#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
//----------------------------------------------------------------------//
#include "TextureBuffer.h"
//----------------------------------------------------------------------//

class CTextureObject {
public:
	CTextureObject();
	~CTextureObject();
private:
	void Initialize();
	UINT GetOpenGLTextureFormat(UINT bpp);
	CTextureBuffer Buffer;
private:
	UINT TexID;
	UINT Width;
	UINT Height;
	UINT Format; // <--- GL_LUMINANCE, GL_RGB or GL_RGBA
public:
	UINT GetWidth();
	UINT GetHeight();
	UINT GetFormat();
	UINT GetTexID();
public:
	bool Create(UINT w, UINT h, UINT bpp);
	void Update(BYTE *y, BYTE *u, BYTE *v);
	void Free();
public:
	CTextureBuffer* GetTextureBuffer(){return &Buffer;}
};