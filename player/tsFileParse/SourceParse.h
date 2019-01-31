#pragma once
#ifndef _SOURCEPARSE_H
#define _SOURCEPARSE_H
#include "amcomdef.h"
#include "StreamReadMgr.h"
#include "DemuxerMgr.h"
#include "common.h"
class SourceParse
{
public:
	SourceParse();
	MBool Open(MPChar strUrl);
	MBool Seek(MInt64 seekTimeStamp);
	MBool ReadFrame(AVPkt** pkt);
	MBool HasVideo() {
		return m_parseFrame.HasVideo();
	}

	MBool HasAudio() {
		return m_parseFrame.HasAudio();
	}


private:
	StreamReadMgr	m_sourceFrame;
	DemuxerMgr	m_parseFrame;
	MPChar		m_pBuffer;
	MInt32		m_iBufferCanReadSize;
	MBool		m_hasVideo;
	MBool		m_hasAudio;

};




#endif // !_SOURCEPARSE_H