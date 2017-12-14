#pragma once

#include <Container/THashMap.h>
#include <Core/Lock/TMutexLock.h>
#include "TSession.h"

class TSession;
class TSessionMan
{
public:
	void AddSession(int sid, TSession *session)
	{
		//add session
		m_lock.Lock();
		m_session.Put(sid, session);
		m_lock.Unlock();

		OnAddSession(sid);
	}
	void DelSession(int sid, TSession * session)
	{
		OnDelSession(sid);

		m_lock.Lock();
		m_session.Remove(sid);
		m_lock.Unlock();
	}


	void AbortSession(int sid, TSession * session)
	{
		OnAbortSession(sid);
	}


	TSession *GetSession(int sid)
	{
		TSession** pSession = m_session.Find(sid);
		if (pSession == nullptr) return nullptr;
		return *pSession;
	}


	bool Send(int id, const TProtocol *protocol, bool urgent)
	{
		bool rc = false;	
		m_lock.Lock();
		TSession *session = GetSession(id);
		if (session) rc = session->Send(protocol, urgent);
		m_lock.Unlock();
		return rc;
	}

	bool Send(int id, const TProtocol &protocol, bool urgent) { return Send(id, &protocol, urgent); }

	virtual void OnAddSession(int sid) = 0;
	virtual void OnDelSession(int sid) = 0;
	virtual void OnAbortSession(int sid) {}

private:
	THashMap<int, TSession*> m_session;
	TMutexLock m_lock;
};

