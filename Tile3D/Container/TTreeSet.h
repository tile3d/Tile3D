#pragma once

#include "TRBTree.h"

template <typename KEY> class TTreeSet
{
public:
	bool Insert(KEY & key) {
		return tm.Insert(key, key);
	}

	TRBTreeNode * Find(const KEY & key) {
		return tm.Find(key);
	}

	bool Remove(const KEY & key) {
		return tm.Remove(key);
	}

	void Remove(TRBTreeNode * pNode) {
		return tm.Remove(pNode);
	}

	bool IsEmtpy() {
		return tm.IsEmpty();
	}

	int Size() {
		return tm.Size();
	}

	void Clear() {
		return tm.Clear();
	}
private:
	TTreeMap<KEY, KEY> tm;
};

