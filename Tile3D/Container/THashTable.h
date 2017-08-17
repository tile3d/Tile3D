#pragma once

template<typename KEY_TYPE, typename VALUE_TYPE> struct TPair {
	KEY_TYPE key;
	VALUE_TYPE value;
};

template<typename KEY_TYPE, typename VALUE_TYPE> struct THashNode
{
	KEY_TYPE m_key;
	VALUE_TYPE m_value;
	THashNode * m_pNext;

	THashNode() {
		m_pNext = nullptr;
	}

	THashNode(KEY_TYPE & key, VALUE_TYPE & value) {
		m_key = key;
		m_value = value;
		m_pNext = nullptr;
	}
};


//TBD
//1) how to support user-defined type?
//
struct HashFunc
{
	HashFunc() {}
	HashFunc(const HashFunc & hf) {}

	unsigned int operator() (int x) const { return x; }
	unsigned int operator() (short x) const { return x; }
	unsigned int operator() (char x) const { return x; }
	unsigned int operator() (long x) const { return x; }

	unsigned int operator() (unsigned int x) const { return x; }
	unsigned int operator() (unsigned short x) const { return x; }
	unsigned int operator() (unsigned char x) const { return x; }
	unsigned int operator() (unsigned long x) const { return x; }

	unsigned int operator() (char * x) const { 
		unsigned int r = 0;
		for (; *x; ++x) {
			r = 5 * r + *x;
		}
		return r;
	}
	
	unsigned int operator() (const char * x) const { 
		unsigned int r = 0;
		for (; *x; ++x) {
			r = 5 * r + *x;
		}
		return r;
	}
};

//
//TBD
//1) need support load factor for rehash
//2) if need support multimap
//
//
template<typename KEY_TYPE, typename VALUE_TYPE> class THashTable
{
public:
	enum {
		HASH_TABLE_INIT_SIZE = 12,
	};

public:
	THashTable() {
		m_buckets = new (THashNode*)[HASH_TABLE_INIT_SIZE];
		memset(m_buckets, 0, sizeof(THashNode*)*HASH_TABLE_INIT_SIZE);
		m_bucketSize = HASH_TABLE_INIT_SIZE;
		m_count = 0;
	}

	~THashTable() {
		ClearBuckets(m_buckets, m_bucketSize);
	}


	bool Put(KEY_TYPE & key, VALUE_TYPE & value) {
		if (m_count >= m_buckets * 2) {
			ReHash(m_buckets * 2);
		}

		int hashValue = GetHashIndex(key);

		//Try to find if exist, if exist return false
		THashNode * pHead = m_buckets[hashValue];
		THashNode * pNode = pHead;
		while (pNode != nullptr) {
			if (pNode->m_key == key) {
				return false;
			}
			else {
				pNode = pNode->m_pNext;
			}
		}

		//if not exist, add it to the bucket list
		Add(key, value, hashValue);
		return true;
	}


	bool Replace(KEY_TYPE & key, VALUE_TYPE & value) {
		if (m_count >= m_buckets * 2) {
			ReHash(m_buckets * 2);
		}

		int hashValue = GetHashIndex(key);

		//Try to find if exist, replace it if exist
		THashNode * pHead = m_buckets[hashValue];
		THashNode * pNode = pHead;
		while (pNode != nullptr) {
			if (pNode->m_key == key) {
				pNode->m_value = value;
			}
			else {
				pNode = pNode->m_pNext;
			}
		}

		Add(key, value, hashValue);
	}

	VALUE_TYPE * Find(KEY_TYPE & key) {
		int hashValue = GetHashIndex(key);
		THashNode * pNode = m_buckets[hashValue];
		while (pNode != nullptr) {
			if (pNode->m_key == key) {
				return &(pNode->m_value);
			}
			else {
				pNode = pNode->m_pNext;
			}
		}
		return nullptr;
	}

	bool Remove(KEY_TYPE  &  key) {
		int hashIndex = GetHashIndex(key);
		THashNode * pBucket = m_buckets[hashIndex];
		THashNode * pNode = pBucket;
		THashNode * pPrevNode = nullptr;
		while (pNode != nullptr) {
			if (pNode->m_key == key) {
				if (pNode == pBucket) {
					pBucket = pNode->m_pNext;
				}
				else {
					pPrevNode->m_pNext = pNode->m_pNext;
				}
				delete pNode;
				--m_count;
				return true;
			}
			else {
				pPrevNode = pNode;
				pNode = pNode->m_pNext;
			}
		}
		return false;
	}


	THashNode * GetHead() {
		for (int i = 0; i < m_bucketSize; ++i) {
			if (m_buckets[i] != nullptr) {
				return m_buckets[i];
			}
		}
		return nullptr;
	}

	THashNode * GetNext(THashNode * pHashNode) {
		if (pHashNode == nullptr) return nullptr;

		THashNode * pNext = pHashNode->m_pNext;
		if (pNext != nullptr) {
			return pNext;
		}
		else {
			KEY key = pHashNode->m_key;
			int hashIndex = GetHashIndex(key);
			for (int i = hashIndex; i < m_bucketSize; ++i) {
				if (m_buckets[i] != nullptr) {
					return m_buckets[i];
				}
			}
		}
		return nullptr;
	}

private:
	unsigned int GetHashIndex(KEY_TYPE & key) {
		return HashFunc(key) % m_bucketSize;
	}

	void Rehash(int newSize) {
		THashNode ** pOldBuckets = m_buckets;
		int oldBucketSize = m_bucketSize;

		m_buckets = new (THashNode*)[newSize];
		memset(m_buckets, 0, sizeof(THashNode*)*newSize);
		m_bucketSize = newSize;

		for (int i = 0; i < oldBucketSize; ++oldBucketSize) {
			THashNode * pNode = pOldBuckets[i];
			while (pNode != nullptr) {
				pNode = pNode->m_pNext;
				Add(pNode->m_key, pNode->m_value, GetHashIndex(pNode->m_key);
			}
		}

		ClearBuckets(pOldBuckets, oldBucketSize);
	}

	void ClearBuckets(THashNode ** buckets, int bucket_size)
	{
		for (int i = 0; i < bucket_size; ++bucket_size) {
			THashNode * pNode = buckets[i];
			while (pNode != nullptr) {
				THashNode * pPrevNode = pNode;
				pNode = pNode->m_pNext;
				delete pPrevNode;
			}
		}
	}

	void Add(KEY_TYPE & key, VALUE_TYPE & value, int hashValue) {
		THashNode * pHead = m_buckets[hashValue];
		//if not exist, add it to the bucket list
		THashNode * pNewNode = new HashNode(key, value);
		pNewNode->m_pNext = pHead->m_pNext;
		pHead = pNewNode;
		++m_count;
	}

private:	
	THashNode ** m_buckets;
	int m_bucketSize;
	int m_count;
};

