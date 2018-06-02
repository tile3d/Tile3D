#pragma once

#include <Core/TObject.h>
#include <Core/Lock/TAtomicInt.h>
#include <Core/Lock/TAtomicLong.h>
#include <Core/Lock/TMutexLock.h>
#include <Container/THashMap.h>


//Note: 
//1) timer precision: 10ms (1 tick)
//2) if the caller call the AddTimer, then the caller deleted, how to handle this case (how to delete the timer)?

//TBD
// 1) optimize the data structure to sort the timernode by the timer interval to reduce the loop times (array list)
// 2) send message to the caller instead of invoke the callback (user end)
// 3) better lock mechanism
//


enum class TimerStatus : int
{
	TIMER_STATUS_INVALID,
	TIMER_STATUS_RUNNING,
	TIMER_STATUS_PAUSED,
};


class TTimerNode : public TObject
{
public:
	TTimerNode() {
		m_pPrev = m_pNext = nullptr;

		m_timerID = 0;
		m_timerStatus = TimerStatus::TIMER_STATUS_INVALID;
		m_nextTick = 0;

		m_interval = 0;
		m_repeat = false;
		m_repeatCount = 0;

	}

	TTimerNode(int interval, bool repeat, int repeatCount) {
		m_pPrev = m_pNext = nullptr;

		m_timerID = 0;
		m_timerStatus = TimerStatus::TIMER_STATUS_INVALID;
		m_nextTick = 0;

		m_interval = interval;
		m_repeat = repeat;
		m_repeatCount = repeatCount;
	}


public:
	TTimerNode * m_pPrev;
	TTimerNode * m_pNext;

	int m_timerID;
	TimerStatus m_timerStatus;
	int m_nextTick;

	int m_interval;	
	bool m_repeat;
	int m_repeatCount;

	void * m_CallbackArg;
	void* (*m_pCallback) (void * pArg);
};

class TTimerMan
{
public:

	enum {
		TICK_INTERVAL = 10,		//10ms per tick
	};

	TTimerMan* GetInstance() {
		static TTimerMan tm;
		return &tm;
	}
	
	int AddTimer(int interval, bool repeat, int repeatCount);

	void RemoveTimer(TTimerNode * pNode);
	void RemoveTimer(int timerID);

	void PauseTimer(TTimerNode * pNode);
	void PauseTimer(int timerID);

	void ResumeTimer(TTimerNode * pNode);
	void ResumeTimer(int timerID);

	void ChangeInterval(TTimerNode * pNode, int new_interval);
	void ChangeInterval(int timerID, int new_interval);

	int GetNextID() { return m_currentID.Increment(); }
	int64 GetCurTick() { return m_tick.Get(); }

	TTimerNode * GetTimerByID(int timerID) {
		TTimerNode ** ppNode = m_nodes.Find(timerID);
		return *ppNode;
	}

	int GetInterval(int timerID);

	void Run();

	void Shutdown();

private:
	void AddTimer(TTimerNode * pNode);
	void ClearTimer(TTimerNode * pNode);
	void StopTimer(TTimerNode * pNode);
	void ContinueTimer(TTimerNode * pNode);
	void ChangeTimerInterval(TTimerNode * pNode, int new_interval);
	void ClearAllTimer();

private:
	TTimerMan() {
		m_pHead = nullptr;
		m_shutdownFlag = false;
	}

	~TTimerMan() {

	}

private:
	TTimerNode * m_pHead;
	THashMap<int, TTimerNode*> m_nodes;

	TMutexLock m_lock;
	TAtomicInt m_currentID;
	TAtomicLong m_tick;

	bool m_shutdownFlag;
};



