#pragma once
#ifndef _IAUDIOPLAYAAC_H
#define _IAUDIOPLAYAAC_H
#include "amcomdef.h"
#include "MKernel.h"
#include "Player.h"
//#if _MSC_VER>=1900  
//#include "stdio.h"   
//_ACRTIMP_ALT FILE* __cdecl __acrt_iob_func(unsigned);
//#ifdef __cplusplus   
//extern "C"
//#endif   
//FILE* __cdecl __iob_func(unsigned i) {
//	return __acrt_iob_func(i);
//}
//#endif /* _MSC_VER>=1900 */  


extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include <SDL/SDL.h>
};
class AudioPlayAAC
{
public:
	AudioPlayAAC();
	virtual MBool Open();
	virtual MVoid Close();
	virtual MBool Display(MPChar pBuffer,MInt32 bufferSize);
	MVoid SetPlayer(Player* player)
	{
		m_player = player;
	};

	MBool Start();
	static void  fill_audio(void *udata, Uint8 *stream, int len);
	MVoid	SetSampleRate(MInt32 sampleRate);
	MVoid	SetSampleSize(MInt32 frameSize);
	static MDWord run(MVoid* lpPara);

	MVoid decode();
private:
	//SDL_AudioSpec	m_wanted_spec;
	MInt32			m_out_channel_layout;
	MInt32			m_out_sample_rate;
	MInt32			m_out_nb_samples;
	AVSampleFormat	m_out_sample_fmt;

	Uint32  audio_len;
	Uint8  *audio_pos;

	MInt32  audio_len_tmp;
	Uint8  *audio_pos_tmp;

	MBool	m_bPlay;
	Player*		m_player;

	MHandle			m_threadHandleRun;
	MBool			m_bRun;
};



#endif // !_IAUDIOLAYAAC_H