#pragma once
#ifndef _AUDIOSCALE_H
#define _AUDIOSCALE_H
#include "amcomdef.h"
#include "mv3File.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include "libswresample/swresample.h"
};

typedef struct _AudioInfo
{
	MInt32			sample_rate;
	MInt32			channels;
	MInt32			sample_fmt;
	MInt32			nb_samples;

}AudioInfo;
class AudioScale
{
public:
	AudioScale();
	MBool Open();
	MVoid Close();
	static MBool isEqual(AudioInfo& a, AudioInfo& b);
	MVoid SetInAudioIndo(AudioInfo	audioInfo);
	MVoid SetOutAudioIndo(AudioInfo	audioInfo);
	AVFrame* Scale(AVFrame *in_pFrame);
private:
	SwrContext *au_convert_ctx;
	AudioInfo	m_in_audio;
	AudioInfo	m_out_audio;

	AVFrame		*m_pFrame;
	mv3File		audioFile;
	MInt32		m_count;

	MInt32		m_pcmBufferSize;
	//uint8_t			*out_buffer;
};
#endif // !_AUDIOSCALE_H