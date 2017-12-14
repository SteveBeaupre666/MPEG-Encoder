#pragma once
//----------------------------------------------------------------------//
#pragma comment(lib, "winmm.lib")
//----------------------------------------------------------------------//
#include <Windows.h>
//----------------------------------------------------------------------//

//-----------------------------------------------------------------------------
// Timer class
//-----------------------------------------------------------------------------
class CTimer {
public:
	CTimer();
protected:
	bool    IsPerfCounterAvailable;
	float   TimeScale;
	__int64 PerfCounterFrequency;
	__int64 LastTime;
	__int64 CurrentTime;
public:
	void  Reset();
	float Tick();

	virtual void OnReset(){}
	virtual void OnTick(float ElapsedTime){}
};

//-----------------------------------------------------------------------------
// Game Timer class
//-----------------------------------------------------------------------------
class CGameTimer : public CTimer {
protected:
	float FPS;
	float FPSElapsedTime;
	int   Frame;
public:
	float GetFPS();
	void  OnReset();
	void  OnTick(float elapsed_time);
};
