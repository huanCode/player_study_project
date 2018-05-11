#include "stdafx.h"
#include "AudioPlayAAC.h"

MBool AudioPlayAAC::Open()
{



	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		return MFalse;
	}

	m_out_channel_layout = AV_CH_LAYOUT_STEREO;
	m_out_sample_fmt = AV_SAMPLE_FMT_S16;
	m_out_sample_rate = 44100;
	MInt32 out_channels = av_get_channel_layout_nb_channels(m_out_channel_layout);
	//Out Buffer Size
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, m_out_nb_samples, m_out_sample_fmt, 1);
	m_wanted_spec.freq = m_out_sample_rate;
	m_wanted_spec.format = AUDIO_S16SYS;
	m_wanted_spec.channels = out_channels;
	m_wanted_spec.silence = 0;
	m_wanted_spec.samples = m_out_nb_samples;
	m_wanted_spec.callback = MNull;
	m_wanted_spec.userdata = MNull;

	if (SDL_OpenAudio(&m_wanted_spec, NULL)<0) {
		printf("can't open audio.\n");
		return -1;
	}

}


MVoid AudioPlayAAC::Close()
{

}

MBool AudioPlayAAC::Display(MPChar pBuffer)
{
	return MTrue;
}

MVoid	AudioPlayAAC::SetSampleRate(MInt32 sampleRate)
{
	m_out_sample_rate = sampleRate;
}

MVoid	AudioPlayAAC::SetSampleSize(MInt32 frameSize)
{
	m_out_nb_samples = frameSize;
}