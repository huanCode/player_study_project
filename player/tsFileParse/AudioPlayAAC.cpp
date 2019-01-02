#include "stdafx.h"
#include "AudioPlayAAC.h"


#if _MSC_VER>=1900  
#include "stdio.h"   
_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
#ifdef __cplusplus   
extern "C"
#endif   
FILE* __cdecl __iob_func(unsigned i) {
	return __acrt_iob_func(i);
}
#endif /* _MSC_VER>=1900 */  


extern "C"
{
#include <SDL/SDL.h>
};

#define DUFAULT_SAMPLE_RATE 44100;

SDL_AudioSpec	m_wanted_spec;
AudioPlayAAC::AudioPlayAAC()
{
	audio_len = 0;
	audio_pos = 0;
	m_out_nb_samples = 1024;
}


MBool AudioPlayAAC::Open()
{



	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		return MFalse;
	}

	m_out_channel_layout = AV_CH_LAYOUT_STEREO;
	m_out_sample_fmt = AV_SAMPLE_FMT_S16;
	m_out_sample_rate = DUFAULT_SAMPLE_RATE;
	MInt32 out_channels = av_get_channel_layout_nb_channels(m_out_channel_layout);
	//Out Buffer Size
	int out_buffer_size = av_samples_get_buffer_size(NULL, out_channels, m_out_nb_samples, m_out_sample_fmt, 1);
	m_wanted_spec.freq = m_out_sample_rate;
	m_wanted_spec.format = AUDIO_S16SYS;
	m_wanted_spec.channels = out_channels;
	m_wanted_spec.silence = 0;
	m_wanted_spec.samples = m_out_nb_samples;
	m_wanted_spec.callback = AudioPlayAAC::fill_audio;
	m_wanted_spec.userdata = this;

	if (SDL_OpenAudio(&m_wanted_spec, NULL)<0) {
		printf("can't open audio.\n");
		return MFalse;
	}
	m_bRun = MFalse;
	m_bPlay = MFalse;

	MInt32 bufferSize = 2048;
	audio_pos_tmp = (Uint8*)MMemAlloc(MNull, bufferSize);
	audio_pos = (Uint8*)MMemAlloc(MNull, bufferSize);
	MMemSet(audio_pos_tmp,0, bufferSize);
	MMemSet(audio_pos, 0, bufferSize);
	return MTrue;

}


MVoid AudioPlayAAC::Close()
{
	SDL_CloseAudio();//Close SDL
	SDL_Quit();
}




void  AudioPlayAAC::fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0

	AudioPlayAAC* pHandle = (AudioPlayAAC*)udata;
	if (!pHandle)
	{
		return;
	}
	pHandle->audio_len = pHandle->audio_len_tmp;
	pHandle->audio_pos = pHandle->audio_pos_tmp;

	SDL_memset(stream, 0, len);
	if (pHandle->audio_len == 0)		/*  Only  play  if  we  have  data  left  */
		return;
	len = (len > pHandle->audio_len ? pHandle->audio_len : len);	/*  Mix  as  much  data  as  possible  */

	SDL_MixAudio(stream, pHandle->audio_pos, len, SDL_MIX_MAXVOLUME);
	pHandle->audio_pos += len;
	pHandle->audio_len -= len;

	pHandle->m_bPlay = MFalse;
}

MDWord AudioPlayAAC::run(MVoid* lpPara)
{
	AudioPlayAAC* audioPlay = (AudioPlayAAC*)lpPara;
	if (!audioPlay)
	{
		return 0;
	}
	audioPlay->decode();
	return 0;

}

MInt32 AudioPlayAAC::GetPerFrameDuration()
{
	MInt32 duration = (m_out_nb_samples * 1000) / DUFAULT_SAMPLE_RATE;
	return duration;
}

MVoid AudioPlayAAC::decode()
{
	m_bRun = MTrue;
	if (!m_player)
	{
		return;
	}

	while (m_bRun)
	{
		if (!m_player->AudioDecode((MPChar)audio_pos_tmp, audio_len_tmp))
		{
			return;
		}

		while(audio_len)
		{
			SDL_Delay(1);
		}

		MMemCpy(audio_pos, audio_pos_tmp, audio_len_tmp);
		audio_len = audio_len_tmp;
		SDL_PauseAudio(0);
	}
}

MBool AudioPlayAAC::Start()
{
	if (!m_bRun)
	{
		m_threadHandleRun = MThreadCreate(run, this);
		if (!m_threadHandleRun)
		{
			return MFalse;
		}
		MThreadResume(m_threadHandleRun);

	}

	return MTrue;
}


MBool AudioPlayAAC::Display(MPChar pBuffer, MInt32 bufferSize)
{
	//while(audio_len)
	//{
	//	SDL_Delay(1);
	//}
	//audio_len = bufferSize;
	//audio_pos = (Uint8 *)pBuffer;
	while(m_bPlay)
	{
		SDL_Delay(1);
	}

	audio_len_tmp = bufferSize;
	audio_pos_tmp = (Uint8 *)pBuffer;
	SDL_PauseAudio(0);

	m_bPlay = MTrue;


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