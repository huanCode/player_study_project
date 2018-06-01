#pragma once
#ifndef _DECODEH264_H
#define _DECODEH264_H
#include "amcomdef.h"
#include "IDecode.h"
extern "C"
{
#include "libavcodec/avcodec.h"
};
class DecodeH264:public IDecode
{
public:
	DecodeH264();
	MBool	Open();
	MVoid	Close();

	MVoid*	DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize);
private:
	AVCodecContext *m_pCodecCtx;
	AVPacket	m_packet;
	AVFrame		*m_pFrame;

};



#endif // !_DECODEH264_H