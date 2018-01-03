#pragma once
#include <time.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

class Time
{
public:
	Time();
	~Time();

	void StartTimer();
	double GetTime();
	float GetDeltaTime();
	int GetFPS();

	

private:
	float currentTime = 0;
	float previousTime = 0;
	float deltaTime = 0;

	double countsPerSecond = 0.0;
	__int64 CounterStart = 0;

	int frameCount = 0;
	int fps = 0;

	__int64 frameTimeOld = 0;
	double frameTime;
};

