#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//

class CTextureBuffer {
public:
	CTextureBuffer();
	~CTextureBuffer();
private:
	BYTE *buf;
private:
	UINT BPP;
	UINT Pitch;
	UINT Width;
	UINT Height;
	UINT NumPixels;
	UINT BufferSize;
private:
	void Initialize();
	UINT GetNextPowerOfTwo(UINT n);
public:
	bool IsAllocated();
	bool Allocate(UINT w, UINT h, UINT bpp);
	void Erase();
	void Free();
public:
	BYTE* Get(){return buf;}
	UINT  GetBPP(){return BPP;}
	UINT  GetPitch(){return Pitch;}
	UINT  GetWidth(){return Width;}
	UINT  GetHeight(){return Height;}
	UINT  GetNumPixels(){return NumPixels;}
	UINT  GetBufferSize(){return BufferSize;}
};

