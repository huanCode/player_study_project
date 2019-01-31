#include "stdafx.h"
#include "Player.h"
#include "SourceParse.h"
//#include "AudioPlayWindow.h"
//#include "VideoPlayWindow.h"
#include "PlatformConfig.h"
#include "DecodecMgr.h"
//#include <windows.h>
#define BUFFER_TIME		3000  //5s
//#define 

Player::Player()
{
	m_pSourceParse = MNull;
	m_bRun = MFalse;
	m_bExit = MFalse;
	m_bufferTime = BUFFER_TIME;
	m_bufferPercent = 0;

	m_bFirstFrameAudio = MFalse;
	m_bFirstFrameVideo = MFalse;


	m_pFrameVideo = MNull;
	m_pFrameAudio = MNull;

	m_bInitDecode = MFalse;
	m_currentAudioTime = 0;
	m_currentVideoTime = 0;

	m_bFirstVideoKeyFrame = MFalse;
	m_bFirstAudioKeyFrame = MFalse;

	m_bHasVideo = MFalse;
	m_bHasAudio = MFalse;

	m_lastAudioTime = -1;
	m_lastVideoTime = -1;

	m_videoInfo.width = 1280;
	m_videoInfo.height = 720;

	m_threadHandleRun = MNull;

	m_bSeek = MFalse;

	m_bForbidRead = MTrue;

}


MVoid Player::create_action(PlayerAction actionState, MBool bClear, MInt64 seekTimeStamp)
{

	PtrPlayActionData action = new PlayActionData();
	action->m_action = actionState;
	action->seekTimeStamp = seekTimeStamp;
	m_lockAction.Lock();
	if (bClear)
	{
		m_arrayPlayActionData.Clear();
	}


	m_arrayPlayActionData.AddNode(action);
	m_lockAction.UnLock();

}

MBool Player::Start(MPChar strUrl)
{
	if (!strUrl)
	{
		return MFalse;
	}


	if (!m_bRun && !m_threadHandleRun)
	{
		RETURN_FALSE(m_lockAction.Init())

		m_strURL.StrCat(strUrl);
		m_context.Init(this);
		create_action(actionStart, MTrue);
		m_threadHandleRun = MThreadCreate(run, this);
		if (!m_threadHandleRun)
		{
			return MFalse;
		}
		MThreadResume(m_threadHandleRun);
	}
	else
	{
		create_action(actionStart);
	}



	return MTrue;
}



MVoid Player::Stop()
{
	if (m_bRun)
	{
		create_action(actionStop, MTrue);
	}
	//m_action = actionStop;
}

MVoid Player::Pause()
{
	if (m_bRun)
	{
		//printf("Pause\r\n");
		create_action(actionPause);
	}
	//m_action = actionPause;
}

MVoid Player::Seek(MInt64 seekTime)
{
	//m_action = actionSeek;
	if (m_bRun)
	{
		//printf("seek\r\n");
		create_action(actionSeek,MFalse, seekTime);
	}
	//m_seekTimeStamp = seekTime;
}


MVoid Player::Close()
{

}


MBool	Player::HasVideo()
{
	if (m_pSourceParse)
	{
		m_pSourceParse->HasAudio();
	}

	return MFalse;
}

MBool	Player::HasAudio()
{
	if (m_pSourceParse)
	{
		m_pSourceParse->HasAudio();
	}

	return MFalse;
}



MDWord Player::run(MVoid* lpPara)
{
	Player* pPlayer = (Player*)lpPara;
	pPlayer->handle();

	return 0;
}


MDWord Player::run_read(MVoid* lpPara)
{
	Player* pPlayer = (Player*)lpPara;
	pPlayer->thread_read();

	return 0;
}

MVoid Player::handle()
{
	printf("run_hand ++\r\n");
	m_bRun = MTrue;
	PlayerAction	m_action = actionNone;

	MInt64  seekTimeStamp = 0;
	MBool ret = MFalse;
	while (m_bRun)
	{
		m_lockAction.Lock();
		if (m_arrayPlayActionData.GetSize() > 0)
		{
			PtrPlayActionData actionTmp = m_arrayPlayActionData.GetNodeAndDelByIndex(1);
			m_action = actionTmp->m_action;
			seekTimeStamp = actionTmp->seekTimeStamp;
			delete actionTmp;

		}
		//printf("run_hand %s\r\n", m_context.GetStateName());
		m_lockAction.UnLock();
		switch (m_action)
		{
		case actionStart:
			m_context.Start();
			break;
		case actionStop:
			m_context.Stop();
			m_bRun = MFalse;
			break;
		case actionPause:
			//printf("run_hand Pause\r\n");
			m_context.Pause();
			break;
		case actionSeek:
			//printf("run_hand Seek\r\n");
			m_context.Seek(seekTimeStamp);
			break;
		default:
			m_bRun = MFalse;
			break;
		}

		if (!m_bExit)
		{
			create_action(actionStop, MTrue);
		}
	}
}

MVoid Player::thread_read()
{



	MInt32 videoPercent = 0;
	MInt32 audioPercent = 0;
	AVPkt* pktHead = MNull;
	AVPkt* pktTail = MNull;
	AVPkt* pkt = MNull;

	MBool ret = MFalse;
	while (m_bExit)
	{
		if (m_bForbidRead)
		{
			MSleep(5);
			continue;
		}
		m_lockSource.Lock();
		ret = m_pSourceParse->ReadFrame(&pkt);
		m_lockSource.UnLock();
		if (!ret)
		{
			m_bExit = MFalse;
			break;
		}
		if (pkt->mediaType == AV_MEDIA_TYPE_VIDEO)
		{
			MMutexLock(m_hMutexVideo);
			m_arrayVideo.AddNode(pkt);
			MMutexUnlock(m_hMutexVideo);
		}
		else if (pkt->mediaType == AV_MEDIA_TYPE_AUDIO)
		{
			MMutexLock(m_hMutexAudio);
			m_arrayAudio.AddNode(pkt);
			MMutexUnlock(m_hMutexAudio);
		}

		
		do{
			if (m_bHasVideo)
			{
				videoPercent = 0;
				MMutexLock(m_hMutexVideo);
				if (m_arrayVideo.GetSize() > 2)
				{
					pktHead = m_arrayVideo.GetHeadNode();
					pktTail = m_arrayVideo.GetTailNode();

					MInt64 duration = pktTail->pts - pktHead->pts;
					videoPercent = (duration * 100) / m_bufferTime;
				}
				MMutexUnlock(m_hMutexVideo);
			}

			if (m_bHasAudio)
			{
				audioPercent = 0;
				MMutexLock(m_hMutexAudio);
				if (m_arrayAudio.GetSize() > 2)
				{
					pktHead = m_arrayAudio.GetHeadNode();
					pktTail = m_arrayAudio.GetTailNode();

					MInt64 duration = pktTail->pts - pktHead->pts;
					audioPercent = (duration * 100) / m_bufferTime;
				}
				MMutexUnlock(m_hMutexAudio);
			}
			//选择比较大的
			m_bufferPercent = videoPercent > audioPercent ? audioPercent : videoPercent;
			if (m_bufferPercent >= 100)
			{
				m_bufferPercent = 100;
				MThreadSleep(m_threadHandleRead, 5);
			}

			audioPercent = videoPercent = 0;

		} while (m_bufferPercent >= 100);



	}

	//m_bRunRead = MFalse;
	//m_context.SetState(Stoping);
}

MVoid Player::State_Restart()
{
	m_audioPlay->ReStart();
	m_avsync.Restart();
}

MVoid Player::State_Stoping()
{

}


MBool Player::prepare()
{
	MDWord timeDuration = 0;
	MDWord timeBegin = MGetCurTimeStamp();
	//1、初始化数据读取
	if (!m_pSourceParse)
	{
		m_pSourceParse = new SourceParse();
		RETURN_FALSE(m_pSourceParse)
		RETURN_FALSE(m_pSourceParse->Open(m_strURL.Get()))

	}
	timeDuration = MGetCurTimeStamp() - timeBegin;
	
	printf("prepare cost 0 time = %d ms\r\n", timeDuration);
	
	//2、初始化解码
	

	RETURN_FALSE(initDecode())

	//3、初始化播放
	timeDuration = MGetCurTimeStamp();
	if (m_pSourceParse->HasAudio())
	{
		m_audioPlay = PlatformFactory::CreateAudioPlay();
		RETURN_FALSE(m_audioPlay)
		RETURN_FALSE(m_audioPlay->Open())

	}
	timeDuration = MGetCurTimeStamp() - timeDuration;
	printf("prepare cost 1 time = %d ms\r\n", timeDuration);
	if (m_pSourceParse->HasVideo())
	{
		m_videoPlay = PlatformFactory::CreateDisplayVideo();
		RETURN_FALSE(m_videoPlay)
		m_videoPlay->SetVideoInfo(&m_videoInfo);
		RETURN_FALSE(m_videoPlay->Open())

	}

	//创建读取数据线程
	if (!m_bExit)
	{
		m_bExit = MTrue;
		m_threadHandleRead = MThreadCreate(run_read, this);
		if (!m_threadHandleRead)
		{
			m_bExit = MFalse;
			return MFalse;
		}
		MThreadResume(m_threadHandleRead);
		
	}

	
	m_hMutexVideo = MMutexCreate();
	m_hMutexAudio = MMutexCreate();


	m_bHasVideo = m_pSourceParse->HasVideo();
	m_bHasAudio = m_pSourceParse->HasAudio();

	m_bForbidRead = MFalse;
	MDWord timeEnd = MGetCurTimeStamp();
	timeDuration = timeEnd - timeBegin;
	printf("prepare cost end time = %d ms\r\n", timeDuration);
	return MTrue;
}



MVoid Player::SetAudioPts(MInt64 pts)
{
	m_avsync.SetCurrentAudioTime(pts);
}

MBool	 Player::AudioDecode(MPChar buffer, MInt32& bufferSize, MInt64* out_pts)
{
	MMutexLock(m_hMutexAudio);
	if (m_arrayAudio.GetSize()==0)
	{
		//m_context.Buffer();
		MMutexUnlock(m_hMutexAudio);
		return MTrue;
	}

	AVPkt* pktFirst = m_arrayAudio.GetNodeAndDelByIndex(1);
	
	//m_avsync.SetCurrentAudioTime(pktFirst->pts);
	m_pFrameAudio = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
	MMutexUnlock(m_hMutexAudio);
	if (!m_pFrameAudio)
	{
		m_bExit = MFalse;
		return MFalse;
	}
	


	MMemCpy(buffer, m_pFrameAudio->pBuffer, m_pFrameAudio->iBufferSize);
	bufferSize = m_pFrameAudio->iBufferSize;
	m_bFirstFrameAudio = MFalse;
	*out_pts = pktFirst->dts;
	//m_currentAudioTime = m_pFrameAudio->pts;
	return MTrue;

}

MVoid Player::Playing_to_Buffering()
{
	m_audioPlay->Pause();
	m_avsync.Pause();
}


State Player::PlayOneFrame()
{
	//MInt32 ret = State::None;
	m_bForbidRead = MFalse;
	if (!m_arrayVideo.GetSize() || !m_arrayAudio.GetSize())
	{
		
		return State::Buffering;
	}

	AVPkt* videoParseFrame = MNull;
	AVPkt* audioParseFrame = MNull;


	//获取第一帧的pts
	MInt64	audioBeginTime = 0;

	if (!m_bFirstVideoKeyFrame && m_bHasVideo)
	{
		AVPkt* tmp = MNull;
		while (m_arrayVideo.GetSize())
		{
			if (m_arrayVideo.GetHeadNode()->bIsSync)
			{
				m_lastVideoTime = m_arrayVideo.GetHeadNode()->pts;
				audioBeginTime  = m_arrayVideo.GetHeadNode()->pts;

				m_avsync.SetFirstVideoTime(m_lastVideoTime);
				m_bFirstVideoKeyFrame = MTrue;
				m_time = MGetCurTimeStamp();
				break;
			}
			else
			{
				tmp = m_arrayVideo.GetNodeAndDelByIndex(1);
				delete tmp;
				tmp = MNull;

			}
		}

		if (m_arrayVideo.GetSize()==0)
		{
			
			return State::Buffering;
		}
	}


	if (!m_bFirstAudioKeyFrame &&m_bHasAudio)
	{
		AVPkt* tmp = MNull;
		while (m_arrayAudio.GetSize())
		{
			if (m_arrayAudio.GetHeadNode()->pts >= audioBeginTime)
			{
				m_currentAudioTime = m_arrayAudio.GetHeadNode()->pts;
				m_bFirstAudioKeyFrame = MTrue;
				break;
			}
			else
			{
				tmp = m_arrayAudio.GetNodeAndDelByIndex(1);
				delete tmp;
				tmp = MNull;

			}
		}

		if (m_arrayAudio.GetSize() == 0)
		{
			return State::Buffering;
		}
	}


	AVPkt*	pktVideo = m_arrayVideo.GetNodeAndDelByIndex(1);
	m_pFrameVideo = m_pDecodeVideo->DecodeFrame(pktVideo->bufferPkt, pktVideo->bufferPktSize, pktVideo->pts, pktVideo->dts);
	//printf("video pts = %d", pktVideo->pts);
	if (!m_pFrameVideo)
	{
		return State::Stoping;
	}
	else if (m_pFrameVideo && !m_pFrameVideo->bSuccess)
	{
		return State::Playing;
	}

	m_currentVideoTime = m_pFrameVideo->pts;

	if (m_currentVideoTime == m_lastVideoTime)
	{	
		//表示解第一帧
		m_audioPlay->SetInterface(this);
		if (!m_audioPlay->Start())
		{
			return State::Stoping;
		}
		
	}


	if (m_avsync.Adjust(m_pFrameVideo->pts))
	{
		MInt64 CurTimeStampAudio = MGetCurTimeStamp();

		MInt64 diffTime = CurTimeStampAudio - m_time;
		m_time = MGetCurTimeStamp();
		m_videoPlay->Display(m_pFrameVideo->pBuffer);
	}
	//m_videoPlay->Display(m_pFrameVideo->pBuffer);

	return State::Playing;

}

MBool Player::initDecode()
{

	if (m_pSourceParse->HasVideo())
	{
		m_pDecodeVideo = new DecodecMgr();
		RETURN_FALSE(m_pDecodeVideo)
		RETURN_FALSE(m_pDecodeVideo->Open(_AV_CODEC_ID_H264))
	}

	if (m_pSourceParse->HasAudio())
	{
		m_pDecodeAudio = new DecodecMgr();
		RETURN_FALSE(m_pDecodeAudio)
		RETURN_FALSE(m_pDecodeAudio->Open(_AV_CODEC_ID_AAC))
	}

	return MTrue;
}


MInt32 Player::buffer()
{
	m_bForbidRead = MFalse;
	MInt32 videoPercent = 0;
	MInt32 audioPercent = 0;

	AVPkt* pktHead = MNull;
	AVPkt* pktTail = MNull;
	if (m_bHasVideo)
	{
		videoPercent = 0;
		MMutexLock(m_hMutexVideo);
		if (m_arrayVideo.GetSize() > 2)
		{
			pktHead = m_arrayVideo.GetHeadNode();
			pktTail = m_arrayVideo.GetTailNode();

			MInt64 duration = pktTail->pts - pktHead->pts;
			videoPercent = (duration * 100) / m_bufferTime;
		}
		MMutexUnlock(m_hMutexVideo);
	}

	if (m_bHasAudio)
	{
		audioPercent = 0;
		MMutexLock(m_hMutexAudio);
		if (m_arrayAudio.GetSize() > 2)
		{
			pktHead = m_arrayAudio.GetHeadNode();
			pktTail = m_arrayAudio.GetTailNode();

			MInt64 duration = pktTail->pts - pktHead->pts;
			audioPercent = (duration * 100) / m_bufferTime;
		}
		MMutexUnlock(m_hMutexAudio);
	}
	//选择比较大的
	MInt64 bufferPercent = videoPercent > audioPercent ? audioPercent : videoPercent;
	if (bufferPercent < 100)
	{
		MSleep(10);
	}
	printf("buffer = %d\r\n", bufferPercent);
	return bufferPercent;
}

MBool Player::State_Seeking(MInt64 seekTime)
{
	m_bForbidRead = MTrue;
	//1、先查找缓存区的
	AVPkt* pktHead = MNull;
	MInt64 baseTime = seekTime;

	MBool isSeek = MFalse;
	MBool isFlushAudio = MTrue;
	m_bSeek = MTrue;
	if (m_bHasVideo)
	{
		m_bFirstVideoKeyFrame = MFalse;
		MMutexLock(m_hMutexVideo);
		
		while(m_arrayVideo.GetSize())
		{
			pktHead = m_arrayVideo.GetHeadNodeDel();
			if (pktHead->bIsSync && pktHead->pts >= baseTime)
			{
				baseTime = pktHead->pts;
				break;
			}
			else
			{
				delete pktHead;
				pktHead = MNull;
			}
			
		}

		if (m_arrayVideo.GetSize() == 0)
		{
			isSeek = MTrue;
		}
		MMutexUnlock(m_hMutexVideo);
	}

	if (m_bHasAudio)
	{
		m_bFirstAudioKeyFrame = MFalse;
		MMutexLock(m_hMutexAudio);
		while (m_arrayAudio.GetSize())
		{
			pktHead = m_arrayAudio.GetHeadNodeDel();
			if (pktHead->bIsSync && pktHead->pts >= baseTime && !isFlushAudio)
			{
				baseTime = pktHead->pts;
				break;
			}
			else
			{
				delete pktHead;
				pktHead = MNull;
			}

		}

		MMutexUnlock(m_hMutexAudio);
	}
	m_lockSource.Lock();
	MBool ret =  m_pSourceParse->Seek(seekTime);
	m_lockSource.UnLock();
	m_pDecodeVideo->Flush_buffers();
	return ret;
}


MBool Player::Pauseing_to_Seeking(MInt64 seekTimeStamp)
{
	MBool ret = MFalse;
	m_lockSource.Lock();
	AVPkt* pkt = MNull;
	m_pDecodeVideo->Flush_buffers();
	MBool isRead = MFalse;
	MInt64 baseTimeStamp = seekTimeStamp;
	while (m_bExit)
	{
		ret = m_pSourceParse->ReadFrame(&pkt);
		if (!ret)
		{
			m_bExit = MFalse;
			break;
		}

		if (!isRead)
		{
			baseTimeStamp = pkt->pts;
			isRead = MTrue;
		}

		if (!isRead && pkt->pts >= baseTimeStamp && pkt->mediaType == AV_MEDIA_TYPE_VIDEO && pkt->bIsSync)
		{
			isRead = MTrue;
		}

		if(isRead)
		{
			if (pkt->mediaType == AV_MEDIA_TYPE_VIDEO)
			{
				MMutexLock(m_hMutexVideo);
				m_arrayVideo.AddNode(pkt);
				MMutexUnlock(m_hMutexVideo);

				m_pFrameVideo = m_pDecodeVideo->DecodeFrame(pkt->bufferPkt, pkt->bufferPktSize, pkt->pts, pkt->dts);
				if (!m_pFrameVideo)
				{
					ret = MFalse;
					m_bExit = MFalse;
					break;

				}
				else if (m_pFrameVideo && m_pFrameVideo->bSuccess && m_pFrameVideo->pts >= baseTimeStamp)
				{
					m_videoPlay->Display(m_pFrameVideo->pBuffer);
					ret = MTrue;
					break;
				}



			}
			else if (pkt->mediaType == AV_MEDIA_TYPE_AUDIO)
			{
				MMutexLock(m_hMutexAudio);
				m_arrayAudio.AddNode(pkt);
				MMutexUnlock(m_hMutexAudio);
			}
		}
		else
		{
			delete pkt;
			pkt = MNull;
		}


	}


	m_pDecodeVideo->Flush_buffers();
	m_lockSource.UnLock();

	return ret;
}


MBool Player::State_Pauseing()
{
	m_bForbidRead = MTrue;
	m_audioPlay->Pause();
	m_avsync.Pause();
	return MTrue;
}
