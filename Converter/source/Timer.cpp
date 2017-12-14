#include "Timer.h"

//-----------------------------------------------------------------------------
// Constructor...
//-----------------------------------------------------------------------------
CTimer::CTimer()
{
	//Check if a High resolution timer is available 
	if(QueryPerformanceFrequency((LARGE_INTEGER*)&PerfCounterFrequency)){ 
		IsPerfCounterAvailable = true;
		QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime); 
		TimeScale = 1.0f / PerfCounterFrequency;
	} else { 
		IsPerfCounterAvailable = false;
		CurrentTime = timeGetTime(); 
		TimeScale	= 0.001f;
    } 
	
	Reset();
}

//-----------------------------------------------------------------------------
// Reset the timer
//-----------------------------------------------------------------------------
void CTimer::Reset()
{
	if(IsPerfCounterAvailable){
		QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	} else {
		CurrentTime = timeGetTime();
	}

	// Initialization
	LastTime = CurrentTime;
}

//-----------------------------------------------------------------------------
// Return the elapsed time since last call
//-----------------------------------------------------------------------------
float CTimer::Tick()
{
	LastTime = CurrentTime;

	if(IsPerfCounterAvailable){
		QueryPerformanceCounter((LARGE_INTEGER*)&CurrentTime);
	} else {
		CurrentTime = timeGetTime();
	}

	// Calculate the elapsed time
	float ElapsedTime = (CurrentTime - LastTime) * TimeScale;

	return ElapsedTime;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Return number of frame recorded in one second
//-----------------------------------------------------------------------------
float CGameTimer::GetFPS()
{
	return FPS;
}

//-----------------------------------------------------------------------------
// Extend Reset()
//-----------------------------------------------------------------------------
void CGameTimer::OnReset()
{
	FPS = 0;
	FPSElapsedTime = 0.0f;
	Frame = 0;
}

//-----------------------------------------------------------------------------
// Extend Tick()
//-----------------------------------------------------------------------------
void CGameTimer::OnTick(float ElapsedTime)
{
	Frame++;
	FPSElapsedTime += ElapsedTime;
	if(FPSElapsedTime >= 1.0f){
		FPS = (float)Frame / FPSElapsedTime;
		Frame = 0;
		while(FPSElapsedTime >= 1.0f)
			FPSElapsedTime -= 1.0f;
	}
}

