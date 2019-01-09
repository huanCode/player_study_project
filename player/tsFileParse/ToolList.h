#pragma once
#ifndef _TOOLLIST_H
#define _TOOLLIST_H
#include "amcomdef.h"
//#include "MKernel.h"


template<class Node>
class ToolList
{
public:
	ToolList();
	~ToolList();
	MBool AddNode(Node pNode);
	MVoid Clear();
	MInt32 GetSize() {
		return m_iSize;
	};

	Node GetNodePtrByIndex(MInt32 index);
	Node GetNodeAndDelByIndex(MInt32 index);

	//MVoid GetNodeAndDelNodeByIndex(MInt32 index);
	//PtrUploadInfo GetNodeAndDel(mv3Upload* pUpload);
	Node GetHeadNode();
	Node GetTailNode();

	Node GetHeadNodeDel();


	MVoid 		DeleteLastNode();

private:
	typedef struct NodeMgr
	{
		NodeMgr()
		{
			next = MNull;
		}
		~NodeMgr()
		{
			if (data)
			{
				delete data;
			}
			
		}
		Node data;
		NodeMgr* next;
	}*NodePtr;
private:
	NodePtr		m_pHead;
	NodePtr		m_pTail;
	MInt32		m_iSize;

	MHandle		m_pLock;
};

template<class Node>
ToolList<Node>::ToolList()
{
	m_pHead = MNull;
	m_pTail = MNull;
	m_iSize = 0;
	//m_pLock = MMutexCreate();
}

template<class Node>
ToolList<Node>::~ToolList()
{
	Clear();
	//MMutexDestroy(m_pLock);
	//MVLOGD("[upload] ~~~~~~ mv3UploadInfoOperator::~mv3UploadInfoOperator\r\n");
}

template<class Node>
MBool ToolList<Node>::AddNode(Node pNode)
{
	NodeMgr* unit = new NodeMgr();
	if (!unit)
	{
		return MFalse;
	}

	//MMutexLock(m_pLock);
	if (!m_pHead)
	{
		
		unit->data = pNode;
		m_pHead = unit;
		m_pTail = unit;
	}
	else
	{
		
		unit->data = pNode;
		m_pTail->next = unit;
		m_pTail = unit;
	}
	m_iSize++;
	//MMutexUnlock(m_pLock);
	return MTrue;

}

template<class Node>
Node ToolList<Node>::GetHeadNode()
{
	return m_pHead->data;
}

template<class Node>
Node ToolList<Node>::GetTailNode()
{
	return m_pTail->data;
}

template<class Node>
Node ToolList<Node>::GetHeadNodeDel()
{
	return GetNodeAndDelByIndex(1);
}




template<class Node>
MVoid 	ToolList<Node>::DeleteLastNode()
{
	NodePtr ret = MNull;
	if (m_pHead)
	{
		ret = m_pHead;
		m_pHead = ret->next;

		m_iSize--;
		if (m_iSize == 0)
		{
			m_pTail = MNull;
		}
	}

	if (ret)
	{
		//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::DeleteLastNode delete\r\n");
		delete ret;
		ret = MNull;
	}


}

template<class Node>
MVoid ToolList<Node>::Clear()
{
	NodePtr pTemp = MNull;
	while (m_pHead)
	{
		pTemp = m_pHead;
		m_pHead = m_pHead->next;
		delete pTemp;
		pTemp = MNull;
	}

	m_pTail = MNull;
	m_iSize = 0;

	m_pHead = MNull;
}

template<class Node>
Node ToolList<Node>::GetNodePtrByIndex(MInt32 index)
{
	NodePtr pCur = m_pHead;
	if (index >= 1)
	{
		for (MInt32 i = 1; index <= m_iSize; i++)
		{
			if (index == i && pCur != MNull)
			{
				return pCur->data;
			}
			else {
				pCur = pCur->next;
			}
		}
	}

	return MNull;
}

//template<class Node>
//MVoid ToolList<Node>::GetNodeAndDelNodeByIndex(MInt32 index)
//{
//	Node data = GetNodeAndDelByIndex(index);
//}


template<class Node>
Node ToolList<Node>::GetNodeAndDelByIndex(MInt32 index)
{
	//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex ++,size = %d\r\n",m_iSize);
	NodePtr pPre = MNull;
	NodePtr pCur = m_pHead;

	for (MInt32 i = 1; index <= m_iSize; i++)
	{
		if (index == i)
		{
			if (pPre)
			{
				//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex 1,not first\r\n");
				pPre->next = pCur->next;
			}
			else
			{
				//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex 2,first\r\n");
				m_pHead = pCur->next;

			}

			if (m_pTail == pCur)
			{
				//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex 3 ,only 1 node\r\n");
				m_pTail = MNull;
			}
			m_iSize--;
			//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex 4 ,m_iSize = %d\r\n",m_iSize);
			return pCur->data;
		}
		else
		{
			//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex 5 ,not match\r\n");
			pPre = pCur;
			pCur = pCur->next;
		}

	}

	//MVLOGD("[upload] mv3UploadInfoOperator<PtrNodeType>::GetNodeAndDelByIndex -- is null,size = %d\r\n",m_iSize);
	return MNull;
}







#endif // !_TOOLSTRING_H