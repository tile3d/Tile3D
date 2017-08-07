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

	TRBTreeNode() {
		m_color = COLOR_RED,
		m_parent = nullptr;
		m_left = nullptr;
		m_right = nullptr;
	}

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

		m_count = 0;
	}

	//TBD
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

	void LeftRotate(TRBTreeNode * pNode);
	void RightRotate(TRBTreeNode * pNode);

	bool InsertUnique(const KEY & key, const VALUE & value);
	void Insert(TRBTreeNode * x, TRBTreeNode * y, const KEY & key, const VALUE & value);

	void Rebalance(TRBTreeNode * pNode);

private:
	TRBTreeNode * CreateNode(const KEY & key, const VALUE & value);

private:
	TRBTreeNode *  m_head;
	int	m_count;
};

template<typename KEY, typename VALUE> bool TRBTree::CreateNode(const KEY & key, const VALUE & value)
{
	TRBTreeNode * pNode = new TRBTreeNode();
	pNode->m_key = key;
	pNode->m_value = value;
	return pNode;
}


template<typename KEY, typename VALUE> bool TRBTree::LeftRotate(TRBTreeNode * pNode)
{
	TRBTreeNode * pNode2 = pNode->m_right;
	pNode->m_right = pNode->m_left;

	if (pNode2->m_left != nullptr) {
		pNode2->m_left->m_parent = pNode;
	}

	pNode2->m_parent = pNode->m_parent;

	if (pNode == GetRoot()) {
		GetRoot() = pNode2;
	}
	else if (pNode == pNode->m_parent->m_left) {
		pNode->m_parent->m_left = pNode2;
	}
	else if (pNode == pNode->m_parent->m_right) {
		pNode->m_parent->m_right = pNode;
	}

	pNode2->m_left = pNode;
	pNode->m_parent = pNode2;
}

template<typename KEY, typename VALUE> bool TRBTree::RightRotate(TRBTreeNode * pNode)
{
	TRBTreeNode * pNode2 = pNode->m_left;
	pNode->m_left = pNode2->m_right;

	if (pNode2->m_right != nullptr) {
		pNode2->m_right->m_parent = pNode;
	}

	pNode2->m_parent = pNode->m_parent;
	
	if (pNode == GetRoot()) {
		GetRoot() = pNode2;
	}
	else if (pNode = pNode->m_parent->m_right) {
		pNode->m_parent->m_right = pNode2;
	}
	else if (pNode = pNode->m_parent->m_left) {
		pNode->m_parent->m_left = pNode2;
	}

	pNode2->m_right = pNode;
	pNode->m_parent = pNode;
}

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

template<typename KEY, typename VALUE> void TRBTree::Insert(TRBTreeNode * x, TRBTreeNode * y, const KEY & key, const VALUE & value)
{
	TRBTreeNode * z;
	if (y = m_header || x != nullptr || key < y->m_key) {
		z = CreateNode(key, value);
		y->m_left = z;

		if (y == m_header) {
			GetRoot() = z;
			GetRightMost() = z;
		}
		else if (y = GetLeftMost()) {
			GetLeftMost() = z;
		}
	}
	else {
		z = CreateNode(key, value);
		y->m_right = z;
		if (y == GetRightMost()) {
			GetRightMost() = z;
		}
	}

	z->m_parent = y;
	z->m_left = nullptr;
	z->m_right = nullptr;

	Rebalance(z);
	++m_count;
}


template<typename KEY, typename VALUE> void TRBTree::Rebalance(TRBTreeNode * pNode)
{
	pNode->m_color = COLOR_RED;

	TRBTreeNode * pRoot = GetRoot();

	while (pNode != pRoot && pNode->m_parent->m_color == COLOR_RED) {
		if (pNode->m_parent = pNode->m_parent->m_parent->m_left) {
			TRBTreeNode * pNode2 = pNode->m_parent->m_parent->m_right;
			if (pNode2 && pNode2->m_color == COLOR_RED) {
				pNode->m_parent->m_color = COLOR_BLACK;
				pNode->m_color = COLOR_BLACK;
				pNode = pNode->m_parent->m_parent;
			}
			else {
				if (pNode = pNode->m_parent->m_right) {
					pNode = pNode->m_parent;
					LeftRotate(pNode);
				}

				pNode->m_parent->m_color = COLOR_BLACK;
				pNode->m_parent->m_parent->m_color = COLOR_RED;
				RightRotate(pNode->m_parent->m_parent);
			}
		}
		else {
			TRBTreeNode * pNode2 = pNode->m_parent->m_parent->m_left;
			if (pNode2 && pNode2->m_color == COLOR_RED) {
				pNode->m_parent->m_color = COLOR_BLACK;
				pNode2->m_color = COLOR_BLACK;
				pNode->m_parent->m_parent->m_color = COLOR_RED;
				pNode = pNode->m_parent->m_parent;
			}
			else {
				if (pNode == pNode->m_parent->m_left) {
					pNode = pNode->m_parent;
					RightRotate(pNode);
				}
				pNode->m_parent->m_color = COLOR_BLACK;
				pNode->m_parent->m_parent->m_color = COLOR_RED;
				LeftRotate(pNode->m_parent->m_parent);
			}
		}
	}
	pRoot->m_color = COLOR_BLOACK;
}

