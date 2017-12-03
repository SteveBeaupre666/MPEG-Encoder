#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
#include <stdio.h>
//----------------------------------------------------------------------//

class CBuffer {
public:
	CBuffer();
	CBuffer(UINT size);
	~CBuffer();
protected:
	BYTE* Buffer;
	UINT  BufferSize;
	void  Initialize();
public:
	bool IsAllocated();
	bool Allocate(int size);
	void Erase();
	void Free();

	BYTE* Get();
	UINT Size();
};

