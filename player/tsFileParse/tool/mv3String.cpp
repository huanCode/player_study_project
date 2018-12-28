#include "mv3String.h"
#include "amstring.h"
#include "ammem.h"
//#include "mv3ModuleXTypes.h"
#define MVLOGD
#define MV2_ERR_NONE 0
#define MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH 1
MV3String::MV3String()
	:M_multiple(2)
{
	m_useSize = 0;
	m_size = 0;
	m_capacity = 0;
	m_str = MNull;
	m_strCatState = 0;
}

MV3String::~MV3String()
{
	Release();
}


void MV3String::Release()
{
	if (m_str)
	{
		//MVLOGD("MV3String::Release(),m_str = %d", m_str);
		m_useSize     = 0;
		m_size        = 0;
		m_capacity    = 0;
		MMemFree(MNull, m_str);
		m_str         = MNull;
		m_strCatState = 0;
	}
}


MRESULT MV3String::AllocMem(MInt32 size)
{

	if (m_strCatState == MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH)
	{
		MVLOGD("MV3String::AllocMem,MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH");
		return m_strCatState;
	}


	//MVLOGD("MV3String::AllocMem,size = %d", size);
	Release();
	m_capacity	= size;

	MInt32 trueSize = m_capacity + 1;	//确保最后个字节是 \0
	//MVLOGD("MV3String::AllocMem,trueSize = %d", trueSize);
	m_str = (MPChar)MMemAlloc(MNull, trueSize);
	if (m_str == MNull)
	{
		MVLOGD("MV3String::AllocMem is err,size = %d\r\n",size);
		Release();
		m_strCatState = MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH;
		return MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH;
	}
	MMemSet(m_str,0, trueSize);
	//MVLOGD("MV3String::AllocMem is OK");
	return MV2_ERR_NONE;
}

MPChar MV3String::Get()
{
	return m_str;
}


MBool MV3String::StrCmp(MPChar src,MPChar dst)
{
	if (MStrCmp(src,dst) == 0)
	{
		return MTrue;
	}
	return MFalse;
}


void MV3String::Set(MPChar str)
{

	Release();
	m_str      = str;
	m_size     = MStrLen(str);
	m_capacity = m_size;

}



unsigned char* MV3String::Get_UChar()
{
	return (unsigned char*)m_str;
}

MInt32 MV3String::GetSize()
{
	return m_size;
}

//MRESULT MV3String::dynamicAllocMem(MInt32 size)
//{
//	//if (size > m_capacity)
//	//{
//	//	MInt32 m_trueSize = m_capacity * M_multiple;
//	//	if (size > m_trueSize)
//	//	{
//	//		m_trueSize = size + m_capacity;
//	//	}
//
//	//}
//
//	//MInt32 m_trueSize = m_capacity + 1;	//确保最后个字节是 \0
//	//m_str = (MPChar)MMemAlloc(MNull, m_trueSize);
//	//if (m_str == MNull)
//	//{
//	//	MVLOGD("MV3String::AllocMem is err,size = %d", size);
//	//	Release();
//	//	return MV2_ERR_MEM_NOT_ENGOUGH;
//	//}
//	//MMemSet(m_str, 0, m_trueSize);
//	return MV2_ERR_NONE;
//}

void MV3String::Print()
{
	MVLOGD("MV3String::Print(): %s\r\n", m_str);
}

//MRESULT MV3String::operator=(MPChar content)
//{
//	MRESULT ret = MV2_ERR_NONE;
//	MInt32 contentSize = MStrLen(content);
//	//MVLOGD("MV3String::operator=,contentSize =%d", contentSize);
//	if (contentSize > m_capacity)
//	{
//		MInt32 trueSize = contentSize + m_capacity +1;	//确保最后个字节是 \0
//		//MVLOGD("MV3String::operator=,trueSize =%d", trueSize);
//		MPChar temp = (MPChar)MMemAlloc(MNull, trueSize);
//		if (temp == MNull)
//		{
//			Release();
//			MVLOGD("MV3String::operator=,MMemAlloc temp is err,m_size =%d,m_capacity =%d,contentSize = %d,trueSize = %d\r\n", m_size, m_capacity, contentSize, trueSize);
//			return MV3_ERR_LICENSE_MEM_NOT_ENGOUGH;
//		}
//		MMemSet(temp, 0, trueSize);
//		MMemCpy(temp, m_str, m_size);
//		MMemFree(MNull,m_str);
//		m_str = temp;
//		m_capacity = trueSize - 1;
//	}
//	
//	m_size = contentSize;
//
//
//	MMemCpy(m_str, content, contentSize);
//
//
//	//MVLOGD("MV3String::operator -- ,m_size =%d,m_capacity =%d", m_size, m_capacity);
//	return MV2_ERR_NONE;
//}

MRESULT MV3String::getState()
{
	return m_strCatState;
}



MRESULT MV3String::StrCat(MPCChar content)
{
	if (m_strCatState == MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH)
	{
		MVLOGD("MV3String::StrCat,MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH");
		return m_strCatState;
	}


	//MVLOGD("MV3String::StrCat ++ ,m_size =%d,m_capacity =%d,content = %s", m_size, m_capacity, content);
//    MRESULT ret = MV2_ERR_NONE;
	MInt32 contentSize = MStrLen(content);
	MInt32 totalSize   = contentSize + m_size;

	if (totalSize > m_capacity)
	{
		MInt32 trueSize = m_capacity * M_multiple;

		if (totalSize > trueSize)
		{
			trueSize = totalSize + m_capacity;
		}

		trueSize++;	//确保最后个字节是 \0
		MPChar temp = (MPChar)MMemAlloc(MNull, trueSize);
		if (temp == MNull)
		{
			Release();
			MVLOGD("MV3String::StrCat,MMemAlloc temp is err,m_size = %d,m_capacity = %d,contentSize = %d\r\n", m_size, m_capacity, contentSize);
			m_strCatState = MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH;
			return MV3_ERR_LICENSE_DISABLE_MEM_NOT_ENOUGH;
		}
		MMemSet(temp,0, trueSize);
		MMemCpy(temp,m_str,m_size);
		MMemFree(MNull,m_str);
		m_str      = temp;
		m_capacity = trueSize - 1;

	}

	
	m_size += contentSize;
	MStrCat(m_str, content);
	//MVLOGD("MV3String::StrCat -- ,m_size =%d,m_capacity =%d", m_size, m_capacity);
	return MV2_ERR_NONE;
}

MRESULT MV3String::StrCat(MV3String &str)
{
	MVLOGD("MV3String::StrCat(MV3String &str)\r\n");
	return StrCat(str.Get());
}