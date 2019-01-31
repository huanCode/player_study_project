#pragma once
#ifndef _DEMUXER_MGR_H
#define _DEMUXER_MGR_H
#include "amcomdef.h"
#include "IBaseIo.h"
#include "ToolList.h"
#include "common.h"
class IStreamRead;
class IDemuxer;
class DemuxerMgr
{
public:
	typedef MBool(*read_probe)(MPChar p_buffer, MUInt32 p_size);
	DemuxerMgr();

	MBool FindParse(MPChar pbuf,MInt32 iBufSize);
	MBool	ReadHeader(MPChar strUrl);

	MBool ReadFrame(AVPkt** pkt);
	MVoid SetDataRead(IStreamRead* obj);
	MBool Seek(MInt64 seekTimeStamp);
	MBool	HasVideo();

	MBool	HasAudio();

private:
	IDemuxer*	m_parse;
	//ToolList<read_probe>	m_probeList;

};




#endif // !_DEMUXER_MGR_H