#include "Thread.h"

//-----------------------------------------------------------------------------
// Constructor...
//-----------------------------------------------------------------------------
Thread::Thread()
{
	Initialize();
}

//-----------------------------------------------------------------------------
// Initialize member variables
//-----------------------------------------------------------------------------
void Thread::Initialize()
{
	ThreadID     = 0;
	ThreadHandle = NULL;
	StopEvent    = NULL;
	PauseEvent   = NULL;
}

//-----------------------------------------------------------------------------
// Start a new thread
//-----------------------------------------------------------------------------
bool Thread::Start(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, UINT WaitMode)
{
	// Wait on the other thread to finish if asked to
	if(WaitMode == WAIT_IF_RUNNING)
		Wait();

	// Make sure the thread isn't already running...
	if(!IsRunning()){
		if(StopEvent)
			CloseHandle(StopEvent);
		
		// Create event to stop the read thread and writing loop
		StopEvent  = CreateEvent(NULL, TRUE, FALSE, NULL);
		PauseEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		// Lauch the thread
		ThreadHandle = CreateThread(NULL, 0, lpStartAddress, lpParameter, 0, &ThreadID);	

		// Wait on the thread to finish if asked to
		if(WaitMode == WAIT_FOR_FINISH)
			Wait();

		return true;
	}
	
	return false;
}

//-----------------------------------------------------------------------------
// Wait for the thread to finish
//-----------------------------------------------------------------------------
void Thread::Wait()
{
	WaitForSingleObject(ThreadHandle, INFINITE);
}

//-----------------------------------------------------------------------------
// Set the signal that can (and SHOULD) be used to stop a thread prematurely
//-----------------------------------------------------------------------------
void Thread::Abort()
{
	// This will wait for the read thread to terminate, if running
	ResetEvent(PauseEvent);
	SetEvent(StopEvent);
	Wait();
	ResetEvent(StopEvent);
	
	// Call CloseHandle() on our handles
	CloseHandle(ThreadHandle);
	CloseHandle(StopEvent);
	CloseHandle(PauseEvent);

	// Reinitialize our handles
	Initialize();
}

//-----------------------------------------------------------------------------
// Check if the Abort signal is set
//-----------------------------------------------------------------------------
bool Thread::Aborted()
{
	// Return true if the even is set
	return WaitForSingleObject(StopEvent, 0) == WAIT_OBJECT_0;
}

//-----------------------------------------------------------------------------
// Check if the thread is still running
//-----------------------------------------------------------------------------
bool Thread::IsRunning()
{
	// Return true if the thread is running
	return WaitForSingleObject(ThreadHandle, 0) == WAIT_TIMEOUT;
}

//-----------------------------------------------------------------------------
// Pause the thread execution
//-----------------------------------------------------------------------------
void Thread::Pause()
{
	if(IsRunning())
		SetEvent(PauseEvent);
}

//-----------------------------------------------------------------------------
// Resume thread execution
//-----------------------------------------------------------------------------
void Thread::Resume()
{
	if(IsRunning())
		ResetEvent(PauseEvent);
}

//-----------------------------------------------------------------------------
// Return true if the thread is paused (and still up)
//-----------------------------------------------------------------------------
bool Thread::IsPaused()
{
	if(!IsRunning())
		return false;

	// Return true if the even is set
	return WaitForSingleObject(PauseEvent, 0) == WAIT_OBJECT_0;
}
