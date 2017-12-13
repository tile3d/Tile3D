#pragma once

#include <Container/THashSet.h>

class TPollIO;
class TPollImp;
class TPollMan
{
public:
	static TPollMan* GetInstance() {
		static TPollMan pollman;
		return &pollman;
	}





private:
	THashSet<TPollIO*> m_pollio;
	TPollImp * m_pollImp;
};

