#pragma once

#include "THashTable.h"

template<typename KEY_TYPE, typename VALUE_TYPE> class THashMap
{
public:
	THashMap() {}
	~THashMap() {}

	bool Put(KEY_TYPE & key, VALUE_TYPE & value) {
		return m_ht.Put(key, value);
	}

	bool Put(TPair & pair) {
		return m_ht.Put(pair.m_key, pair.m_value);
	}

	bool Replace(KEY_TYPE & key, VALUE_TYPE & value) {
		return m_ht.Put(key, value);
	}

	VALUE_TYPE * Find(KEY_TYPE & key) {
		return m_ht.Find(key);
	}

	bool Remove(KEY_TYPE  &  key) {
		return m_ht.Remove(key);
	}

	THashNode * GetHead() {
		return m_ht.GetHead();
	}

	THashNode * GetNext(THashNode * pNode) {
		return m_ht.GetNext();
	}

private:
	/** Hidden copy constructor. */
	THashMap(const THashMap&) = delete;

	/** Hidden assignment operator. */
	THashMap& operator=(const THashMap&) = delete;


private:
	THashTable<KEY_TYPE, VALUE_TYPE> m_ht;
};

