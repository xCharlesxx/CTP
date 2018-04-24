#pragma once
#pragma once
#include "CoreMinimal.h"

enum NodeFlags
{
	NODE_OPEN = 0x01,
	NODE_CLOSED = 0x02,
};
typedef uint64 polyRef;
struct CNode
{
	//float pos[3];				///< Position of the node.
	FVector pos; 
	float cost;					///< Cost from previous node to current node.
	float total;				///< Cost up to the node.
	CNode* parentId;		///< Index to parent node.
	unsigned int flags;		///< Node flags 0/open/closed.
	polyRef id;				///< Polygon ref the node corresponds to.
};
class CNodePool
{
public:
	CNodePool(int maxNodes);
	~CNodePool();
	CNode* getNode(polyRef id, bool createIfNoneFound);

	inline unsigned int getNodeIdx(const CNode* node) const
	{
		if (!node) return 0;
		return (unsigned int)(node - m_nodes) + 1;
	}

	inline CNode* getNodeAtPoly(polyRef id)
	{
		if (!id) return 0; 
		for (int i = 0; i < m_nodeCount; i++)
			if (m_nodes[i].id == id)
				return &m_nodes[i];
		return 0; 
	}

	inline CNode* getNodeAtIdx(unsigned int idx)
	{
		if (!idx) return 0;
		return &m_nodes[idx - 1];
	}

	inline const CNode* getNodeAtIdx(unsigned int idx) const
	{
		if (!idx) return 0;
		return &m_nodes[idx - 1];
	}

	//inline int getMemUsed() const
	//{
	//	return sizeof(*this) +
	//		sizeof(dtNode)*m_maxNodes +
	//		sizeof(dtNodeIndex)*m_maxNodes +
	//		sizeof(dtNodeIndex)*m_hashSize;
	//}

	inline int getMaxNodes() const { return m_maxNodes; }
	inline int getNodeCount() const { return m_nodeCount; }

	//inline int getHashSize() const { return m_hashSize; }
	//inline dtNodeIndex getFirst(int bucket) const { return m_first[bucket]; }
	//inline dtNodeIndex getNext(int i) const { return m_next[i]; }

private:

	CNode * m_nodes;
	//dtNodeIndex* m_first;
	//dtNodeIndex* m_next;
	const int m_maxNodes;
	//const int m_hashSize;
	int m_nodeCount;
};

class CNodeQueue
{
public:
	CNodeQueue(int n);
	~CNodeQueue();
	inline void operator=(CNodeQueue&) {}

	inline void clear()
	{
		m_size = 0;
	}

	inline CNode* top()
	{
		return m_heap[0];
	}

	inline CNode* pop()
	{
		CNode* result = m_heap[0];
		m_size--;
		trickleDown(0, m_heap[m_size]);
		return result;
	}

	inline void push(CNode* node)
	{
		m_size++;
		bubbleUp(m_size - 1, node);
	}

	inline void modify(CNode* node)
	{
		for (int i = 0; i < m_size; ++i)
		{
			if (m_heap[i] == node)
			{
				bubbleUp(i, node);
				return;
			}
		}
	}

	inline bool empty() const { return m_size == 0; }

	inline int getMemUsed() const
	{
		return sizeof(*this) +
			sizeof(CNode*)*(m_capacity + 1);
	}

	inline int getCapacity() const { return m_capacity; }

private:
	void bubbleUp(int i, CNode* node);
	void trickleDown(int i, CNode* node);

	CNode** m_heap;
	const int m_capacity;
	int m_size;
};