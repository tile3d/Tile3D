#pragma once


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

	unsigned int operator() (char x) const { return x; }
	unsigned int operator() (short x) const { return x; }
	unsigned int operator() (int x) const { return x; }
	unsigned int operator() (long x) const { return x; }

	unsigned int operator() (unsigned char x) const { return x; }
	unsigned int operator() (unsigned short x) const { return x; }
	unsigned int operator() (unsigned int x) const { return x; }
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
		m_buckets = new THashNode<KEY_TYPE, VALUE_TYPE>*[HASH_TABLE_INIT_SIZE];
		memset(m_buckets, 0, sizeof(THashNode<KEY_TYPE, VALUE_TYPE>*)*HASH_TABLE_INIT_SIZE);
		m_bucketSize = HASH_TABLE_INIT_SIZE;
		m_count = 0;
	}

	~THashTable() {
		ClearBuckets(m_buckets, m_bucketSize);
	}

	void Clear() {
		ClearBuckets(m_buckets, m_bucketSize);
	}

	bool Put(KEY_TYPE & key, VALUE_TYPE & value) {
		if (m_count >= m_bucketSize * 2) {
			Rehash(m_bucketSize * 2);
		}

		int hashValue = GetHashIndex(key);

		//Try to find if exist, if exist return false
		THashNode<KEY_TYPE, VALUE_TYPE> * pHead = m_buckets[hashValue];
		THashNode<KEY_TYPE, VALUE_TYPE> * pNode = pHead;
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
		THashNode<KEY_TYPE, VALUE_TYPE> * pNode = m_buckets[hashValue];
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
		THashNode<KEY_TYPE, VALUE_TYPE> * pBucket = m_buckets[hashIndex];
		THashNode<KEY_TYPE, VALUE_TYPE> * pNode = pBucket;
		THashNode<KEY_TYPE, VALUE_TYPE> * pPrevNode = nullptr;
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


	THashNode<KEY_TYPE, VALUE_TYPE> * GetHead() {
		for (int i = 0; i < m_bucketSize; ++i) {
			if (m_buckets[i] != nullptr) {
				return m_buckets[i];
			}
		}
		return nullptr;
	}

	THashNode<KEY_TYPE, VALUE_TYPE> * GetNext(THashNode<KEY_TYPE, VALUE_TYPE> * pHashNode) {
		if (pHashNode == nullptr) return nullptr;

		THashNode<KEY_TYPE, VALUE_TYPE> * pNext = pHashNode->m_pNext;
		if (pNext != nullptr) {
			return pNext;
		}
		else {
			KEY_TYPE key = pHashNode->m_key;
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
		return hashFunc(key) % m_bucketSize;
	}

	void Rehash(int newSize) {
		THashNode<KEY_TYPE, VALUE_TYPE>  ** pOldBuckets = m_buckets;
		int oldBucketSize = m_bucketSize;

		m_buckets = new THashNode<KEY_TYPE, VALUE_TYPE>*[newSize];
		memset(m_buckets, 0, sizeof(THashNode<KEY_TYPE, VALUE_TYPE>*)*newSize);
		m_bucketSize = newSize;

		for (int i = 0; i < oldBucketSize; ++oldBucketSize) {
			THashNode<KEY_TYPE, VALUE_TYPE> * pNode = pOldBuckets[i];
			while (pNode != nullptr) {
				pNode = pNode->m_pNext;
				Add(pNode->m_key, pNode->m_value, GetHashIndex(pNode->m_key));
			}
		}

		ClearBuckets(pOldBuckets, oldBucketSize);
	}

	void ClearBuckets(THashNode<KEY_TYPE, VALUE_TYPE> ** buckets, int bucket_size)
	{
		THashNode<KEY_TYPE, VALUE_TYPE> * pNode;
		THashNode<KEY_TYPE, VALUE_TYPE> * pPrevNode;
		for (int i = 0; i < bucket_size; ++i) {
			pNode = buckets[i];
			while (pNode != nullptr) {
				pPrevNode = pNode;
				pNode = pNode->m_pNext;
				delete pPrevNode;
			}
		}
		delete[] buckets;
	}

	void Add(KEY_TYPE & key, VALUE_TYPE & value, int hashValue) {
		THashNode<KEY_TYPE, VALUE_TYPE> * pHead = m_buckets[hashValue];

		//if not exist, add it to the bucket list
		THashNode<KEY_TYPE, VALUE_TYPE> * pNewNode = new THashNode<KEY_TYPE, VALUE_TYPE>(key, value);
		pNewNode->m_pNext = pHead->m_pNext;
		pHead = pNewNode;
		++m_count;
	}

private:	
	THashNode<KEY_TYPE, VALUE_TYPE> ** m_buckets;
	int m_bucketSize;
	int m_count;

	HashFunc hashFunc;
};

