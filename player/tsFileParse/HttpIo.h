#pragma once
#ifndef _HTTPIO_H
#define _HTTPIO_H
#include "amcomdef.h"
#include "IBaseIo.h"
class HttpIo:public IBaseIo
{
public:
	HttpIo();

	MBool IoOpen(MPChar strUrl);
	MInt32 IoRead(MPChar pBuf, MDWord dwSize, MInt64 llOffset = 0);
	MVoid IoClose();
	MInt32	GetSize();
	MBool	isComplete();
	MVoid IoGetConfig(MInt32 dwCfgType, MVoid** pdwValue);
private:
	MPChar GetLocationUrl();
private:
	
	MHandle m_hHttp;
	MInt32	m_alreadyReadSize;

};




#endif // !HttpIo