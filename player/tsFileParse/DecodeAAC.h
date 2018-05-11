#pragma once
#ifndef _DECODEAAC_H
#define _DECODEAAC_H
#include "amcomdef.h"
#include "IDecode.h"
#include "AudioScale.h"
extern "C"
{
#include "libavcodec/avcodec.h"
};





class DecodeAAC :public IDecode
{
public:
	DecodeAAC();
	MBool	Open();
	MVoid	Close();

	AVFrame*	DecodeFrame(MPChar srcBuffer, MInt32 srcBufferSize);
	
private:
	AVCodecContext *m_pCodecCtx;
	AVPacket	m_packet;
	AVFrame		*m_pFrame;
	AudioInfo	m_in_audio;
	AudioInfo	m_out_audio;

	AudioScale	*m_pAudioScale;

};
#endif // !_DECODEAAC_H