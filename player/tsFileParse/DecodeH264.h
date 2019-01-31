#pragma once
#ifndef _DECODEH264_H
#define _DECODEH264_H
#include "amcomdef.h"
#include "IDecode.h"
extern "C"
{
#include <libswscale/swscale.h>
#include "libavcodec/avcodec.h"
};
class DecodeH264:public IDecode
{
public:
	DecodeH264();
	MBool	Open();
	MVoid	Close();

	MVoid SetInfo(MPVoid info);

	Frame*	DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize,MInt64 pts, MInt64 dts);

	MVoid  Flush_buffers();
private:
	MBool OpenSwsCtx();
private:
	AVCodecContext *m_pCodecCtx;
	AVPacket	m_packet;
	AVFrame		*m_pFrame;

	AVFrame		*m_pFrameYUV;
	VideoInfo	m_videoInfo;
	SwsContext *m_img_convert_ctx;

	MBool		m_bSwsInit;

	uint8_t		*m_out_buffer;

	MInt32		m_yuv420pSize;
};



#endif // !_DECODEH264_H