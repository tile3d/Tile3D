#pragma once

#include "THashTable.h"

template<typename KEY_TYPE> class THashSet
{
public:
	THashSet() {}
	~THashSet() {}

	bool Put(KEY_TYPE & key) {
		return m_ht.Put(key, key);
	}

	VALUE_TYPE * Find(KEY_TYPE & key) {
		return m_ht.Find(key);
	}

	bool Remove(KEY_TYPE  &  key) {
		return m_ht.Remove(key);
	}

private:
	/** Hidden copy constructor. */
	THashSet(const THashSet&) = delete;

	/** Hidden assignment operator. */
	THashSet& operator=(const THashSet&) = delete;


private:
	THashTable<KEY_TYPE, KEY_TYPE> m_ht;
};

