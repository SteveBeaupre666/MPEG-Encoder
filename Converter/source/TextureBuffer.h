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
	UINT size;

	UINT BPP;
	UINT Width;
	UINT Pitch;
	UINT Height;

	void Initialize();
	UINT GetNextPowerOfTwo(UINT n);
public:
	bool IsAllocated();
	bool Allocate(UINT w, UINT h, UINT bpp);
	void Erase();
	void Free();

	BYTE* Get(){return buf;}
	UINT  GetBPP(){return BPP;}
	UINT  GetWidth(){return Width;}
	UINT  GetPitch(){return Pitch;}
	UINT  GetHeight(){return Height;}
};

