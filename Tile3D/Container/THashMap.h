#pragma once

#include "THashTable.h"
#include "TPair.h"

//
//1) support operator[]
//2) support append another map
//3) support iterator and range loop
//
template<typename KEY_TYPE, typename VALUE_TYPE> class THashMap
{
public:
	THashMap() {}
	~THashMap() {}

	bool Put(const KEY_TYPE & key, const VALUE_TYPE & value) {
		return m_ht.Put(key, value);
	}

	bool Put(const TPair<KEY_TYPE, VALUE_TYPE>  & pair) {
		return m_ht.Put(pair.m_key, pair.m_value);
	}

	bool Replace(const KEY_TYPE & key, const VALUE_TYPE & value) {
		return m_ht.Put(key, value);
	}

	VALUE_TYPE * Find(const KEY_TYPE & key) {
		return m_ht.Find(key);
	}

	bool Remove(const KEY_TYPE  &  key) {
		return m_ht.Remove(key);
	}

	THashNode<KEY_TYPE, VALUE_TYPE> * GetHead() {
		return m_ht.GetHead();
	}

	THashNode<KEY_TYPE, VALUE_TYPE> * GetNext(THashNode<KEY_TYPE, VALUE_TYPE> * pNode) {
		return m_ht.GetNext(pNode);
	}

	void Clear() {
		return m_ht.Clear();
	}
private:
	/** Hidden copy constructor. */
	THashMap(const THashMap&) = delete;

	/** Hidden assignment operator. */
	THashMap& operator=(const THashMap&) = delete;


private:
	THashTable<KEY_TYPE, VALUE_TYPE> m_ht;
};

