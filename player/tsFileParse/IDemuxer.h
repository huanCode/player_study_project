#pragma once
#ifndef _IDEMUXER_H
#define _IDEMUXER_H
#include "amcomdef.h"
#include "IStreamRead.h"
#include "common.h"
class IDemuxer
{
public:
	IDemuxer()
	{
		m_dataRead = MNull;
	}
	virtual MBool	ReadHeader(MPChar strUrl) = 0;
	//virtual MBool   Init();
	virtual MBool	ReadPacket(AVPkt** pkt) = 0;
	virtual MVoid	Close() = 0;
	virtual MBool   Seek(MInt64 seekTimeStamp) = 0;
	MVoid	SetDataRead(IStreamRead* obj)
	{
		m_dataRead = obj;
	}

	virtual MBool	HasVideo() = 0;

	virtual MBool	HasAudio() = 0;
protected:
	IStreamRead*	m_dataRead;

};




#endif // !_IDEMUXER_H
