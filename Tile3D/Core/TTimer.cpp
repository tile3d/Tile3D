#include "TTimer.h"
#include <Common/TAssert.h>
#include <Common/TTimeUtil.h>

int TTimerMan::AddTimer(int interval, bool repeat, int repeatCount)
{
	TTimerNode * pNode = new TTimerNode(interval, repeat, repeatCount);
	
	TTimerMan* pTimerMan = TTimerMan::GetInstance();
	pNode->m_timerID = pTimerMan->GetNextID();
	pNode->m_timerStatus = TimerStatus::TIMER_STATUS_RUNNING;
	pNode->m_nextTick = pTimerMan->GetCurTick() + pNode->m_interval;

	AddTimer(pNode);
	return pNode->m_timerID;
}

void TTimerMan::AddTimer(TTimerNode * pNode)
{
	m_lock.Lock();
	if (m_pHead == nullptr) {
		m_pHead = pNode;
	}
	else {
		pNode->m_pNext = m_pHead;
		m_pHead->m_pPrev = pNode;
		m_pHead = pNode;
	}
	m_nodes.Put(pNode->m_timerID, pNode);
	m_lock.Unlock();
}

void TTimerMan::RemoveTimer(TTimerNode * pNode)
{
	m_lock.Lock();
	ClearTimer(pNode);
	m_lock.Unlock();
}

void TTimerMan::RemoveTimer(int timerID)
{
	m_lock.Lock();
	TTimerNode * pNode = GetTimerByID(timerID);
	if (pNode != nullptr) {
		ClearTimer(pNode);
	}	
	m_lock.Unlock();
}

void TTimerMan::ClearTimer(TTimerNode * pNode)
{
	TTimerNode * pPrev = pNode->m_pPrev;
	TTimerNode * pNext = pNode->m_pNext;

	if (pPrev != nullptr) {
		pPrev->m_pNext = pNode->m_pNext;

		if (pNext != nullptr) {
			pNext->m_pPrev = pPrev;
		}
	}
	//head node
	else {
		m_pHead = pNext;
		if (pNext != nullptr) {
			pNext->m_pPrev = nullptr;
		}
	}

	m_nodes.Remove(pNode->m_timerID);
	delete pNode;

}

void TTimerMan::PauseTimer(TTimerNode * pNode)
{
	m_lock.Lock();	
	StopTimer(pNode);
	m_lock.Unlock();
}


void TTimerMan::PauseTimer(int timerID)
{
	m_lock.Lock();
	TTimerNode * pNode = GetTimerByID(timerID);
	if (pNode != nullptr) {
		StopTimer(pNode);
	}
	m_lock.Unlock();
}

void TTimerMan::StopTimer(TTimerNode * pNode)
{
	TTimerMan* pTimerMan = TTimerMan::GetInstance();

	pNode->m_timerStatus = TimerStatus::TIMER_STATUS_PAUSED;

	pNode->m_nextTick =  pNode->m_nextTick - pTimerMan->GetCurTick();

	TAssert(pNode->m_nextTick >= 0);

	if (pNode->m_nextTick < 0) {
		pNode->m_nextTick = 0;
	}
}

void TTimerMan::ResumeTimer(TTimerNode * pNode)
{
	m_lock.Lock();
	ContinueTimer(pNode);
	m_lock.Unlock();
}

void TTimerMan::ResumeTimer(int timerID)
{
	m_lock.Lock();
	TTimerNode * pNode = GetTimerByID(timerID);
	if (pNode != nullptr) {
		ContinueTimer(pNode);
	}
	m_lock.Unlock();
}

void TTimerMan::ContinueTimer(TTimerNode * pNode)
{
	TTimerMan* pTimerMan = TTimerMan::GetInstance();

	pNode->m_timerStatus = TimerStatus::TIMER_STATUS_RUNNING;
	pNode->m_nextTick = pTimerMan->GetCurTick() + pNode->m_nextTick;
}

void TTimerMan::ChangeInterval(TTimerNode * pNode, int new_interval)
{
	m_lock.Lock();
	ChangeTimerInterval(pNode, new_interval);
	m_lock.Unlock();
}


void TTimerMan::ChangeInterval(int timerID, int new_interval)
{
	m_lock.Lock();
	TTimerNode * pNode = GetTimerByID(timerID);
	if (pNode != nullptr) {
		ChangeTimerInterval(pNode, new_interval);
	}
	m_lock.Unlock();
}

void TTimerMan::ChangeTimerInterval(TTimerNode * pNode, int new_interval)
{
	pNode->m_interval = new_interval;
}

int TTimerMan::GetInterval(int timerID)
{
	int interval = 0;
	m_lock.Lock();
	TTimerNode * pNode = GetTimerByID(timerID);
	if (pNode != nullptr) {
		interval = pNode->m_interval;
	}
	m_lock.Unlock();
	return interval;
}

void TTimerMan::Run()
{
	while (m_shutdownFlag) {
		TTimeUtil::Sleep(TICK_INTERVAL*1000);

		int cur_tick = m_tick.Increment();
		m_lock.Lock();
		TTimerNode * pCurNode;
		TTimerNode * pNode = m_pHead;
		while (pNode != nullptr) {
			if (pNode->m_timerStatus == TimerStatus::TIMER_STATUS_RUNNING && cur_tick >= pNode->m_nextTick) {
				pNode->m_pCallback(pNode->m_CallbackArg);
				if (pNode->m_repeat) {
					if (pNode->m_repeatCount > 0 || pNode->m_repeatCount == -1) {
						if (pNode->m_repeatCount > 0) --pNode->m_repeatCount;
						pNode->m_nextTick = cur_tick + pNode->m_interval;
					}
					else {
						pCurNode = pNode;
						pNode = pNode->m_pNext;
						ClearTimer(pCurNode);
					}
				}
				else {
					pCurNode = pNode;
					pNode = pNode->m_pNext;
					ClearTimer(pCurNode);
					continue;
				}
			}
			pNode = pNode->m_pNext;
		}
		m_lock.Unlock();
	}
}

void TTimerMan::Shutdown()
{
	m_shutdownFlag = true;

	m_lock.Lock();
	ClearAllTimer();
	m_lock.Unlock();
}

void TTimerMan::ClearAllTimer()
{
	TTimerNode * pNode = m_pHead;
	TTimerNode * pCurNode;
	while (pNode != nullptr) {
		pCurNode = pNode;
		pNode = pNode->m_pNext;
		delete pCurNode;
	}
	m_pHead = nullptr;
}

