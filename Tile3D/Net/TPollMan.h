#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>
#include <Core/Lock/TScopeLock.h>
#include "TPollImp.h"

class TPollIO;
class TPollMan
{
public:
	//TBD: multiple thread safe problem
	static TPollMan* GetInstance() {
		static TPollMan pollman;
		return &pollman;
	}

	TPollMan() {
		m_pollImp = nullptr;
		Init();
	}

	~TPollMan() {
		if (m_pollImp != nullptr) {
			delete m_pollImp;
		}
	}

	void Init();

	void Poll(int timeout);

	TPollIO* FindPollIO(int fd) {
		TScopeLock sl(m_iolock);
		TPollIO** ppPollIO = m_pollio.Find(fd);
		if (ppPollIO) return nullptr;
		return *ppPollIO;
	}

	void AddPollIO(int fd, TPollIO* pPollIO) {
		TScopeLock sl(m_iolock);
		m_pollio.Put(fd, pPollIO);
	}

	void RemovePollIO(int fd, TPollIO* pPollIO) {
		TScopeLock sl(m_iolock);
		m_pollio.Remove(fd);
	}

private:
	THashMap<int, TPollIO*> m_pollio;
	TPollImp * m_pollImp;
	TMutexLock m_iolock;
};

