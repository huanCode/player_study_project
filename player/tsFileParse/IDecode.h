#pragma once
#ifndef _IDECODE_H
#define _IDECODE_H
#include "amcomdef.h"

class IDecode
{
public:
	virtual MBool	Open() = 0;
	virtual MVoid	Close() = 0;

	virtual MVoid*	DecodeFrame(MPChar srcBuffer,MInt32 srcBufferSize,MInt64 pts,MInt64 dts) = 0;


};



#endif // !_IDECODE_H