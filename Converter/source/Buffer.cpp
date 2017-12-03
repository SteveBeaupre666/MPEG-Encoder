#include "Buffer.h"

CBuffer::CBuffer()
{
	Initialize();
}

CBuffer::CBuffer(UINT size)
{
	Initialize();
	Allocate(size);
}

CBuffer::~CBuffer()
{
	Free();
}

void CBuffer::Initialize()
{
	Buffer = NULL;
	BufferSize = 0;
}

bool CBuffer::IsAllocated()
{
	return Buffer != NULL;
}

bool CBuffer::Allocate(int size)
{
	if(size <= 0){
		Free();
		return false;	
	} else if(size == (int)BufferSize){
		Erase();
		return true;
	} else {
		Free();
		Buffer = new BYTE[size];
		if(Buffer){
			BufferSize = size;
			Erase();
			return true;
		} else {
			Initialize();
			return false;
		}
	}
}

void CBuffer::Erase()
{
	if(IsAllocated())
		ZeroMemory(Buffer, BufferSize);
}

void CBuffer::Free()
{
	if(IsAllocated())
		delete [] Buffer;

	Initialize();
}

UINT CBuffer::Size()
{
	return BufferSize;
}

BYTE* CBuffer::Get()
{
	return Buffer;
}

