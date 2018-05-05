#include "TTimeCounter.h"
#include <time.h>

void TTimeCounter::Start()
{
	Reset();
	m_startTime = time(NULL);
}

void TTimeCounter::Pause()
{
	m_pauseTime = time(NULL);
}

void TTimeCounter::Stop()
{
	m_endTime = time(NULL);
}

void TTimeCounter::Continue()
{
	m_continueTime = time(NULL);
}

int TTimeCounter::GetCostTime()
{
	if (m_pauseTime != 0 && m_continueTime != 0) {
		return (int)((m_pauseTime - m_startTime) + (m_endTime - m_continueTime));
	}
	else {
		return (int)(m_endTime - m_startTime);
	}
}
