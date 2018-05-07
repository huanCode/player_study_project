#pragma once
#ifndef _IDECODE_H
#define _IDECODE_H
#include "amcomdef.h"

class IDecode
{
public:
	virtual MBool	Open() = 0;
	virtual MBool	Close() = 0;

	virtual MBool	DecodeFrame(MPChar srcBuffer,MInt32 srcBufferSize) = 0;


};



#endif // !_IDECODE_H