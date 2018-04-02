#pragma once
#ifndef _IDATAREAD_H
#define _IDATAREAD_H
#include "amcomdef.h"
class IDataRead
{
public:
	virtual MBool Open(MPChar strUrl) = 0;
	virtual MBool Read(MChar**, MDWord dwSize, MInt32& out_readSize) = 0;
	virtual MVoid Close() = 0;

};



#endif // !_IDATAREAD_H