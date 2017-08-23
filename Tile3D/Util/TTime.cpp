#include "TTime.h"
#include <time.h>

void TTime::Start()
{
	Reset();
	m_startTime = time(NULL);
}

void TTime::Pause()
{
	m_pauseTime = time(NULL);
}

void TTime::Stop()
{
	m_endTime = time(NULL);
}

void TTime::Continue()
{
	m_continueTime = time(NULL);
}

int TTime::GetCostTime()
{
	if (m_pauseTime != 0 && m_continueTime != 0) {
		return (int)((m_pauseTime - m_startTime) + (m_endTime - m_continueTime));
	}
	else {
		return (int)(m_endTime - m_startTime);
	}
}
