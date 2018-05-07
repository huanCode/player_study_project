#pragma once
#ifndef _DECODEH264_H
#define _DECODEH264_H
#include "amcomdef.h"
extern "C"
{
#include "libavcodec/avcodec.h"
};
class DecodeH264
{
public:
	DecodeH264();
	MBool	Open();
	MBool	Close();

	MBool	DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize);
private:
	AVCodecContext *m_pCodecCtx;
	AVPacket	m_packet;
	AVFrame		*m_pFrame;

};



#endif // !_DECODEH264_H