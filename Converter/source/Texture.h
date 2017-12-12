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

class CTexture {
public:
	CTexture();
	~CTexture();
private:
	void Initialize();
private:
	UINT TexID;
	UINT Width;
	UINT Height;
	UINT Format;
public:
	CTextureBuffer Buffer;

	bool Create(UINT w, UINT h, UINT bpp);
	void Update(BYTE *Y, BYTE *U, BYTE *V);
	void Delete();

	UINT GetTexID(){return TexID;}
	UINT GetWidth(){return Width;}
	UINT GetHeight(){return Height;}
	UINT GetFormat(){return Format;}
};