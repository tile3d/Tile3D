#pragma once

#include <Container/THashMap.h>

class TPollIO;
class TPollImp;
class TPollMan
{
public:
	//TBD: multiple thread safe problem
	static TPollMan* GetInstance() {
		static TPollMan pollman;
		return &pollman;
	}

	TPollIO* FindPollIO(int fd) {
		TPollIO** ppPollIO = m_pollio.Find(fd);
		if (ppPollIO) return nullptr;
		return *ppPollIO;
	}



private:
	THashMap<int, TPollIO*> m_pollio;
	TPollImp * m_pollImp;
};

