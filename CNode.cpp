#include "CNode.h"
#include "Engine/World.h"

CNodePool::CNodePool(int maxNodes) :
	m_nodes(0),
	m_maxNodes(maxNodes),
	m_nodeCount(0)
{
	m_nodes = (CNode*)malloc(sizeof(CNode)*m_maxNodes);
}
CNodePool::~CNodePool()
{
	free(m_nodes); 
}
//Find and return else create new
CNode * CNodePool::getNode(polyRef id, bool createIfNoneFound)
{
	//Find Node
	for (int i = 0; i < m_nodeCount; i++)
		if (m_nodes[i].id == id)
			return &m_nodes[i];

	if (!createIfNoneFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Could not Find Node"));
		return nullptr;
	}
	if (m_nodeCount >= m_maxNodes)
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: Could not Create new Node, maxNode limit reached"));
		return nullptr;
	}

	//Create new node 
	CNode* node = 0; 
	node = &m_nodes[m_nodeCount];
	node->parentId = 0;
	node->cost = 0;
	node->total = 0;
	node->id = id;
	node->flags = 0;

	m_nodeCount++; 

	return node; 
}

void CNodeQueue::trickleDown(int i, CNode * node)
{
	int child = (i * 2) + 1;
	while (child < m_size)
	{
		//If not out of bounds
		if (((child + 1) < m_size) && (m_heap[child]->total > m_heap[child + 1]->total))
		{
			child++;
		}
		m_heap[i] = m_heap[child];
		i = child;
		child = (i * 2) + 1;
	}
	bubbleUp(i, node);
}

CNodeQueue::CNodeQueue(int n) :
	m_heap(0), 
	m_capacity(n), 
	m_size(0)
{
	m_heap = (CNode**)malloc(sizeof(CNode*)*(m_capacity + 1));
}

CNodeQueue::~CNodeQueue()
{
	free(m_heap);
}

void CNodeQueue::bubbleUp(int i, CNode * node)
{
	int parent = (i - 1) / 2;
	//(index > 0) means there is a parent
	while ((i > 0) && (m_heap[parent]->total > node->total))
	{
		//While new node is cheaper to traverse, move up queue
		m_heap[i] = m_heap[parent];
		i = parent;
		parent = (i - 1) / 2;
	}
	m_heap[i] = node;
}

