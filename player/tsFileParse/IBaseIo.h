#pragma once
#ifndef _IBASEIO_H
#define _IBASEIO_H
#include "amcomdef.h"
#include "AllConfig.h"
class IBaseIo
{
public:
	IBaseIo(MPChar strType)
	{
		m_strType = strType;
		m_streamSize = 0;
	}

	virtual MBool IoOpen(MPChar strUrl) = 0;
	virtual MInt32 IoRead(MPChar pBuf, MDWord dwSize, MInt64 llOffset = 0) = 0;
	virtual MVoid  IoClose() = 0;
	virtual MInt32	GetSize() = 0;
	virtual MBool	isComplete() = 0;
	virtual MVoid IoGetConfig(MInt32 dwCfgType, MVoid** pdwValue) = 0;

protected:
	MInt32	m_streamSize;
private:
	MPChar	m_strType;


};



#endif // !_PARSEHLS_H#pragma once