#ifndef _TOOLLISTLOCK_H
#define _TOOLLISTLOCK_H
#include "ToolList.h"
#include "MKernel.h"
class ToolListLock
{

public:
	ToolListLock();


	//MVoid AddNode(Node pNode);
	//MVoid Clear();
	//MInt32 GetSize() {
	//	return m_iSize;
	//};

	//Node GetNodePtrByIndex(MInt32 index);
	//Node GetNodeAndDelByIndex(MInt32 index);
	////PtrUploadInfo GetNodeAndDel(mv3Upload* pUpload);
	//Node GetLastNode();
	//Node GetFirstNode();
	//MVoid 		DeleteLastNode();

private:
	MHandle		m_pLock;

};


#endif // !_TOOLLISTLOCK_H
