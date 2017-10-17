#pragma once

#include <Container/THashMap.h>
#include <Common/TOctetsStream.h>


class TProtocol
{
public:
	TProtocol() : m_type(0), m_sizePolicy(0), m_priorPolicy(0) {}
	~TProtocol() {}

	int GetType() const { return m_type; }
	int GetSizePolicy() const { return m_sizePolicy; }
	int GetPriorPolicy() const { return m_priorPolicy; }

	static TProtocol* GetProtocol(int type) {
		TProtocol** proto = m_protocols.Find(type);
		if (proto == nullptr) return nullptr;
		return *proto;
	}


	void Encode(const TOctetsStream& os) const;


private:
	static THashMap<int, TProtocol*> m_protocols;
	
	int m_type;
	int m_sizePolicy;
	int m_priorPolicy;

};


