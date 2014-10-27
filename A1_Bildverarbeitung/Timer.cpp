#include "Timer.h"
#include <stdexcept>
#ifdef WIN32
#include <Windows.h>
#else
#include  <sys/time.h>
#endif


	Timer::Timer() :
frequency(0), startTime(0),lastTime(0)
	{
#ifdef WIN32
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*> (&frequency));
#else
		frequency = 1000;
#endif
	}

	void Timer::start()
	{
		startTime = getCurrentTime();
		lastTime = startTime;
	}
	float Timer::restart()
	{
		float result = stop();
		start();
		return result;
	}
	 int64_t Timer::getCurrentTime()
	{
#ifdef WIN32
		int64_t  currentTime = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*> (&currentTime));
		return currentTime;
#else
		timeval current;
		if (gettimeofday(&current, 0) != 0)
			throw std::runtime_error("timer error!");
		return current.tv_sec * 1000 + (current.tv_usec) / 1000;
#endif
	}

	float Timer::stop()
	{
		int64_t  current = getCurrentTime();
		 int64_t  elapsed = current - lastTime;
		startTime = 0;
		lastTime = 0;
		return elapsed / (float) frequency;
	}

	float Timer::getElapsedSeconds()
	{
		int64_t  current = getCurrentTime();
		int64_t  elapsed = current - lastTime;
		lastTime = current;
		return elapsed / (float) frequency;
	}

	float Timer::getTotalTime()
	{
		return (getCurrentTime() - startTime) / (float) frequency;
	}

