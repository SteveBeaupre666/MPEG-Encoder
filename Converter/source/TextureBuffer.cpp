#include "TextureBuffer.h"

CTextureBuffer::CTextureBuffer()
{
	Initialize();
}

CTextureBuffer::~CTextureBuffer()
{
	Free();
}

void CTextureBuffer::Initialize()
{
	BPP        = 0;
	Pitch      = 0;
	Width      = 0;
	Height     = 0;
	NumPixels  = 0;
	BufferSize = 0;
	buf        = NULL;
}

UINT CTextureBuffer::GetNextPowerOfTwo(UINT n)
{
	UINT x = 1;
	while(x < n)
		x <<= 1;

	return x;
}

bool CTextureBuffer::IsAllocated()
{
	return buf != NULL;
}

bool CTextureBuffer::Allocate(UINT w, UINT h, UINT bpp)
{
	Free();

	if(!w || !h || !bpp)
		return false;

	
	BPP    = bpp;
	Width  = GetNextPowerOfTwo(w);
	Height = GetNextPowerOfTwo(h);

	Pitch      = Width * BPP;
	NumPixels  = Width * Height;
	BufferSize = Width * Height * BPP;

	buf = new BYTE[BufferSize];
	if(!buf){
		Initialize();
		return false;
	}

	Erase();

	return true;
}

void CTextureBuffer::Erase()
{
	if(IsAllocated())
		ZeroMemory(buf, BufferSize);
}

void CTextureBuffer::Free()
{
	if(IsAllocated())
		delete [] buf;
	
	Initialize();
}
