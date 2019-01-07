#include "AVSync.h"
#include "amkernel.h"
#include <windows.h>
#include <stdio.h>
AVSync::AVSync()
{
	m_lastAudioTime = -1;
	m_currentAudioTime = -1;
	m_lastVideoTime = -1;
	m_lock = MMutexCreate();
	m_CurTimeStampAudio = -1;

	m_bPause = MFalse;
}


MVoid AVSync::Pause()
{
	//if (!m_bPause)
	//{
	//	MMutexLock(m_lock);
	//	MDWord timeDirrerent = MGetCurTimeStamp() - m_CurTimeStampAudio;
	//	m_currentAudioTime += timeDirrerent;
	//	MMutexUnlock(m_lock);
	//	m_bPause = MTrue;
	//}

	MMutexLock(m_lock);
	if (m_CurTimeStampAudio != -1)
	{
		MDWord timeDirrerent = MGetCurTimeStamp() - m_CurTimeStampAudio;
		m_currentAudioTime += timeDirrerent;
	}
	MMutexUnlock(m_lock);
	m_bPause = MTrue;

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
	MInt64 tmp = 9983;
	MInt32 videoSpan = currentVideoTime - m_lastVideoTime;
	if (currentVideoTime > tmp)
	{
		int b = 1;
	}

	MDWord timeDirrerent = MGetCurTimeStamp() - m_CurTimeStampAudio;
	MInt64 realCurrentAudioTime = m_currentAudioTime + timeDirrerent;	//当前音频正在播放的时间
	//printf("currentVideoTime = %lld ,realCurrentAudioTime = %lld \r\n", currentVideoTime,realCurrentAudioTime);
	if (currentVideoTime >= realCurrentAudioTime)
	{
		//video > audio 
		
		sleepTime = currentVideoTime - m_currentAudioTime;
		//printf("sleep = %d \r\n", sleepTime);
		if (sleepTime >= 100)
		{
			int a = 1;
		}
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
