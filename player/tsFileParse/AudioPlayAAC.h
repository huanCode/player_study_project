#pragma once
#ifndef _IAUDIOLAYAAC_H
#define _IAUDIOLAYAAC_H
#include "amcomdef.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "sdl/SDL.h"
};
class AudioPlayAAC
{
public:

	virtual MBool Open() = 0;
	virtual MVoid Close() = 0;
	virtual MBool Display(MPChar pBuffer) = 0;

	MVoid	SetSampleRate(MInt32 sampleRate);
	MVoid	SetSampleSize(MInt32 frameSize);

private:
	SDL_AudioSpec	m_wanted_spec;
	MInt32			m_out_channel_layout;
	MInt32			m_out_sample_rate;
	MInt32			m_out_nb_samples;
	AVSampleFormat	m_out_sample_fmt;
};



#endif // !_IAUDIOLAYAAC_H