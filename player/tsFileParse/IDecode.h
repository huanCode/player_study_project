#pragma once
#ifndef _IDECODE_H
#define _IDECODE_H
#include "amcomdef.h"
#include "common.h"

class IDecode
{
public:
	virtual MBool	Open() = 0;
	virtual MVoid	Close() = 0;
	MVoid   SetPlayer()
	{
	
	}

	virtual MVoid SetInfo(MPVoid info) = 0;

	virtual Frame*	DecodeFrame(MPChar srcBuffer,MInt32 srcBufferSize,MInt64 pts,MInt64 dts) = 0;
	virtual MVoid  Flush_buffers() {};
protected:
	Frame		m_pOneFrame;
};



#endif // !_IDECODE_H