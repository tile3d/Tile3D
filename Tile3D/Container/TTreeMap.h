#pragma once

template <typename KEY, typename VALUE> class TTreeMap
{
public:	
	bool Insert(KEY & key, VALUE & value) {
		return tm.Insert(key, value);
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
	TTreeMap<KEY, VALUE> tm;
};

