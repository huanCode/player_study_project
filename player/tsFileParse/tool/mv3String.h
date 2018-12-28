#ifndef __MV3STRING__H__
#define __MV3STRING__H__
#include "amcomdef.h"

class MV3String
{

public:
	MV3String();
	~MV3String();
	MRESULT AllocMem(MInt32 size);
	MPChar Get();

	//相等返回true
	static MBool StrCmp(MPChar src,MPChar dst);
	MVoid Set(MPChar str);
	unsigned char* Get_UChar();
	MInt32 GetSize();

	//动态扩大
	//MRESULT dynamicAllocMem(MInt32 size);	
	MVoid Print();
	//MRESULT operator=(MPChar content);
	MRESULT	StrCat(MPCChar content);
	MRESULT	StrCat(MV3String &str);
	MVoid Release();

	MRESULT getState();
private:
	
private:
	MInt32	m_size;
	MInt32  m_capacity;
	MInt32  m_useSize;
	MPChar	m_str;

	MRESULT	m_strCatState;
	const MInt32	M_multiple;
};



#endif //__MV3STRING__H__
