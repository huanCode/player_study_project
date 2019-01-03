#include "AVSync.h"
#include "amkernel.h"
#include <windows.h>
AVSync::AVSync()
{
	m_lastAudioTime = -1;
	m_currentAudioTime = -1;
	m_lastVideoTime = -1;
	m_lock = MMutexCreate();
	m_CurTimeStampAudio = MGetCurTimeStamp();
}

MVoid AVSync::SetCurrentAudioTime(MInt64 currentAudioTime)
{
	MMutexLock(m_lock);
	m_currentAudioTime = currentAudioTime;
	m_CurTimeStampAudio = MGetCurTimeStamp();
	m_lastAudioTime = currentAudioTime - m_audioSpan;


	MMutexUnlock(m_lock);
}

MVoid AVSync::SetFirstVideoTime(MInt64 firstVideoTime)
{
	m_lastVideoTime = firstVideoTime;
}

MBool AVSync::Adjust(MInt64 currentVideoTime)
{
	MInt32 sleepTime = 0;
	MBool ret = MTrue;
	MMutexLock(m_lock);

	MInt32 videoSpan = currentVideoTime - m_lastVideoTime;


	MDWord timeDirrerent = MGetCurTimeStamp() - m_CurTimeStampAudio;
	MInt64 realCurrentAudioTime = m_currentAudioTime + timeDirrerent;	//当前音频正在播放的时间

	if (currentVideoTime >= realCurrentAudioTime)
	{
		//video > audio 
		sleepTime = currentVideoTime - m_currentAudioTime;

	}
	else
	{
		//video <
		MInt32 avSpan = m_currentAudioTime - currentVideoTime;
		MInt32 videoSpan = currentVideoTime - m_lastVideoTime;
		
		if (videoSpan < avSpan)
		{
			ret = MFalse;
		}


	}
	m_lastVideoTime = currentVideoTime;
	MMutexUnlock(m_lock);

	if (sleepTime)
	{
		Sleep(sleepTime);
	}

	return ret;

}




MVoid AVSync::SetPerAudioFrameDuration(MInt32 span)
{
	m_audioSpan = span;
}
