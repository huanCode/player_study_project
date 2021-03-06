#include "stdafx.h"
#include "AudioPlayWindow.h"
#include "ammem.h"

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
AudioPlayWindow::AudioPlayWindow()
{
	audio_len = 0;
	audio_pos = 0;
	m_out_nb_samples = 1024;

	m_bPause = MFalse;

	audio_len_tmp = 0;
}


MBool AudioPlayWindow::Open()
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
	m_wanted_spec.callback = AudioPlayWindow::fill_audio;
	m_wanted_spec.userdata = this;
	m_wanted_spec.size = out_buffer_size;
	if (SDL_OpenAudio(&m_wanted_spec, NULL)<0) {
		printf("can't open audio.\n");
		return MFalse;
	}
	m_bRun = MFalse;
	m_bPlay = MFalse;

	MInt32 bufferSize = out_buffer_size; //channel * bytesPerSample * samples = 2channels * stereo * 1024 samples =2 * 2 * 1024
	audio_pos_tmp = (Uint8*)MMemAlloc(MNull, bufferSize);
	MMemSet(audio_pos_tmp,0, bufferSize);

	return MTrue;

}


MVoid AudioPlayWindow::Close()
{
	SDL_CloseAudio();//Close SDL
	SDL_Quit();

	if (audio_pos_tmp)
	{
		MMemFree(MNull, audio_pos_tmp);
		audio_pos_tmp = MNull;
	}


	//m_player = MNull;
}

MVoid	AudioPlayWindow::Pause()
{
	m_bPause = MTrue;
	SDL_PauseAudio(1);
}

MVoid	AudioPlayWindow::ReStart()
{
	m_bPause = MFalse;
	SDL_PauseAudio(0);
}




void  AudioPlayWindow::fill_audio(void *udata, Uint8 *stream, int len) {
	//SDL 2.0

	AudioPlayWindow* pHandle = (AudioPlayWindow*)udata;
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

MDWord AudioPlayWindow::run(MVoid* lpPara)
{
	AudioPlayWindow* audioPlay = (AudioPlayWindow*)lpPara;
	if (!audioPlay)
	{
		return 0;
	}
	audioPlay->decode();
	return 0;

}

MInt32 AudioPlayWindow::GetPerFrameDuration()
{
	MInt32 duration = (m_out_nb_samples * 1000) / DUFAULT_SAMPLE_RATE;
	return duration;
}

MVoid AudioPlayWindow::decode()
{
	m_bRun = MTrue;
	if (!m_interfaceObject)
	{
		return;
	}
	MInt32 delayTimeMS = 5;
	MInt64 audioPts = 0;
	while (m_bRun)
	{
		if (m_bPause)
		{
			SDL_Delay(10);
			continue;
		}

		audio_len_tmp = 0;
		if (!m_interfaceObject->AudioDecode((MPChar)audio_pos_tmp, audio_len_tmp, &audioPts))
		{
			return;
		}
		else if(audio_len_tmp == 0)
		{
			continue;
		}


		while(m_bRun && audio_len)
		{
			delayTimeMS = m_bPause ? 10 : 1;
			SDL_Delay(delayTimeMS);
		}
		m_interfaceObject->SetAudioPts(audioPts);
		//MMemCpy(audio_pos, audio_pos_tmp, audio_len_tmp);
		audio_len = audio_len_tmp;
		SDL_PauseAudio(0);
	}

	int i = 1;
}

MBool AudioPlayWindow::Start()
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


//MBool AudioPlayWindow::Display(MPChar pBuffer, MInt32 bufferSize)
//{
//	//while(audio_len)
//	//{
//	//	SDL_Delay(1);
//	//}
//	//audio_len = bufferSize;
//	//audio_pos = (Uint8 *)pBuffer;
//	while(m_bPlay)
//	{
//		SDL_Delay(1);
//	}
//
//	audio_len_tmp = bufferSize;
//	audio_pos_tmp = (Uint8 *)pBuffer;
//	SDL_PauseAudio(0);
//
//	m_bPlay = MTrue;
//
//
//	return MTrue;
//}

MVoid	AudioPlayWindow::SetSampleRate(MInt32 sampleRate)
{
	m_out_sample_rate = sampleRate;
}

MVoid	AudioPlayWindow::SetSampleSize(MInt32 frameSize)
{
	m_out_nb_samples = frameSize;
}