#ifndef _AVSYNC_H_
#define _AVSYNC_H_
#include "amcomdef.h"
class AVSync
{
public:
	AVSync();
	MVoid SetCurrentAudioTime(MInt64 currentAudioTime);
	MVoid SetFirstVideoTime(MInt64 firstVideoTime);
	MBool Adjust(MInt64 currentVideoTime);
	MVoid SetPerAudioFrameDuration(MInt32 span);
	MVoid Pause();
private:
	MInt64	m_lastAudioTime;
	MInt64	m_currentAudioTime;


	MDWord	m_CurTimeStampAudio;
	MHandle	m_lock;
	MInt32	m_audioSpan;	//播放的每次一段pcm的duration

	MInt64	m_lastVideoTime;


	MBool	m_bPause; //true : pause state
};

#endif //_AVSYNC_H_
