#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
//----------------------------------------------------------------------//

class CThreadLock {
public:
	CThreadLock();
	~CThreadLock();
private:
	CRITICAL_SECTION cs;
public:
	void Lock();
	void Unlock();
};
