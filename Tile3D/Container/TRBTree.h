#pragma once

enum TRBTreeColor
{
	COLOR_RED = false,
	COLOR_BLACK = true
};


template<typename KEY, typename VALUE> struct TRBTreeNode
{
	TRBTreeColor m_color;
	TRBTreeNode * m_parent;
	TRBTreeNode * m_left;
	TRBTreeNode * m_right;
	KEY	m_key;
	VALUE m_value;


	TRBTreeNode * GetLeftMost() {
		TRBTreeNode * pNode = this;
		while (pNode->m_left != nullptr) {
			pNode = pNode->m_left;
		}
		return pNode;
	}

	TRBTreeNode * GetRightMost() {
		TRBTreeNode * pNode = this;
		while (pNode->m_right != nullptr) {
			pNode = pNode->m_right;
		}
		return pNode;
	}

	void Increment() {
		if (this->m_right != nullptr) {
			this = this->m_right;
			while (this->m_left != nullptr) {
				this = this->m_left;
			}
		}
		else {
			TBRTreeNode * y = this->m_parent;
			while (this == y->m_right) {
				this = y;
				y = y->m_parent;
			}
			if (this->m_right != y) {
				this = y;
			}
		}
	}


	void Decrement() {
		if (this->m_color == COLOR_RED && this->m_parent->m_parent = this) {
			this = this->m_right;
		}
		else if (this->m_left != 0) {
			TRBTreeNode * y = this->m_left;
			while (y->m_right != 0) {
				y = y->m_right;
			}
			this = y;
		}
		else {
			TRBTreeNode *y = this->m_parent;
			while (this == y->m_left) {
				this = y;
				y = y->m_parent;
			}
			this = y;
		}
	}


};

template<typename KEY, typename VALUE> class TRBTree
{
public:
	TRBTree() {
		m_head = new TRBTreeNode();

		m_head->m_color = COLOR_RED;
		m_head->m_parent = nullptr;
		m_head->m_left = &this->m_head;
		m_head->m_right = &this->m_head;
	}

	~TRBTree() {
		if (m_head != nullptr) {
			delete m_head;
		}
	}

	TRBTreeNode * GetRoot() { return m_head.m_parent; }
	TRBTreeNode * GetLeftMost() { return m_head.m_left; }
	TRBTreeNode * GetRightMost() { return m_head.m_right; }

	TRBTreeNode * GetLeft(TRBTreeNode * pNode) {
		return pNode->m_left;
	}

	TRBTreeNode * GetRight(TRBTreeNode * pNode) {
		return pNode->m_right;
	}


	bool InsertUnique(const KEY & key, const VALUE & value);
private:
	TRBTreeNode *  m_head;
	int	m_count;
};

template<typename KEY, typename VALUE> bool TRBTree::InsertUnique(const KEY & key, const VALUE & value) 
{
	TRBTreeNode* y = m_head;
	TRBTreeNode* x = GetRoot();

	bool comp = true;
	while (x != 0) {
		y = x;
		comp = key < x->m_key;
		x = comp ? x->m_left : x->m_right;
	}

	TRBTreeNode * j = y;
	if (comp) {
		if (y == GetLeftMost()) {
			Insert(x, y, key, value);
			return true;
		}
		else {
			j->Decrement();
		}
	}

	if (j->m_key < key) {
		Insert(x, y, key, value);
		return true;
	}

	return false;
}


