#pragma once

#include "TTypes.h"

class TTimeCounter
{
public:
	TTimeCounter() {
		Reset();
	}

	void Reset() {
		m_startTime = 0;
		m_endTime = 0;
		m_pauseTime = 0;
		m_continueTime = 0;
	}
	void Start();

	void Pause();

	void Stop();

	void Continue();

	int GetCostTime();

private:
	int64 m_startTime;
	int64 m_endTime;
	int64 m_pauseTime;
	int64 m_continueTime;
};
