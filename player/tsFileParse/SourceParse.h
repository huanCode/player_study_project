#pragma once
#ifndef _SOURCEPARSE_H
#define _SOURCEPARSE_H
#include "amcomdef.h"
#include "SourceFrame.h"
#include "ParseFrame.h"
#include "common.h"
class SourceParse
{
public:
	SourceParse();
	MBool Open(MPChar strUrl);
	MBool ReadFrame(AVPkt** pkt);
	MBool HasVideo() {
		return m_parseFrame.HasVideo();
	}

	MBool HasAudio() {
		return m_parseFrame.HasAudio();
	}


private:
	SourceFrame	m_sourceFrame;
	ParseFrame	m_parseFrame;
	MPChar		m_pBuffer;
	MInt32		m_iBufferCanReadSize;
	MBool		m_hasVideo;
	MBool		m_hasAudio;

};




#endif // !_SOURCEPARSE_H