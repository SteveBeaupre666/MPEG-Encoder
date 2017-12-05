#pragma once
//----------------------------------------------------------------------//
#include <Windows.h>
//----------------------------------------------------------------------//

#define ABORT_IF_RUNNING  0 // Dont start another thread if one is already running
#define WAIT_IF_RUNNING   1 // Wait for the other thread to finish before staring a new one
#define WAIT_FOR_FINISH   2 // Start a new thread and block the calling thread until finish (this one cause the thread handles to be overwriten, rendering further call effective only on the last started thread)

class Thread { 
public:
	Thread();
	~Thread(){}
private:
	DWORD  ThreadID;
	HANDLE ThreadHandle;
	HANDLE StopEvent;
	HANDLE PauseEvent;
	void Initialize();
public:
	HANDLE GetHandle();//{return hThread;}

	bool IsRunning();
	bool Start(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, UINT WaitMode = ABORT_IF_RUNNING);
	
	void Abort();
	bool Aborted();
	void Wait();

	void Pause();
	void Resume();
	bool IsPaused();
};
