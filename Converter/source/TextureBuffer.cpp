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

	buf = NULL;
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

	Pitch  = Width * BPP;
	size   = Pitch * BPP;

	buf = new BYTE[size];

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
		ZeroMemory(buf, size);
}

void CTextureBuffer::Free()
{
	if(IsAllocated())
		delete [] buf;
	
	Initialize();
}
