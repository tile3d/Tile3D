#pragma once

#include "TRBTree.h"

template <typename KEY, typename VALUE> class TTreeMap
{
public:	
	bool Insert(const KEY & key, const VALUE & value) {
		return tm.Insert(key, value);
	}

	TRBTreeNode<KEY, VALUE> * Find(const KEY & key) {
		return tm.Find(key);
	}

	bool Remove(const KEY & key) {
		return tm.Remove(key);
	}

	void Remove(TRBTreeNode<KEY, VALUE> * pNode) {
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
	TRBTree<KEY, VALUE> tm;
};

