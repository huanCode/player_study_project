#pragma once
#ifndef _IPARSE_H
#define _IPARSE_H
#include "amcomdef.h"
#include "IDataRead.h"
class IParse
{
public:
	IParse()
	{
		m_dataRead = MNull;
	}
	virtual MBool	ReadHeader() = 0;
	virtual MBool	ReadPacket() = 0;
	MVoid	SetDataRead(IDataRead* obj)
	{
		m_dataRead = obj;
	}

protected:
	IDataRead*	m_dataRead;

};




#endif // !_IPARSE_H
