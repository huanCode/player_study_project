#include "stdafx.h"
#include "Player.h"
#include "SourceParse.h"
#include "AudioPlayAAC.h"
#include "VideoPlayWindow.h"
#include "DecodecMgr.h"
#define BUFFER_TIME		5000  //5s
//#define 

Player::Player()
{
	m_pSourceParse = MNull;
	m_bRun = MFalse;
	m_bRunRead = MFalse;
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

}

MBool Player::Start(MPChar strUrl)
{
	if (!strUrl)
	{
		return MFalse;
	}

	m_strURL.StrCat(strUrl);


	m_action = actionPlay;
	m_context.Init(this);
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






MVoid Player::Stop()
{
	m_action = actionStop;
}

MVoid Player::Pause()
{
	m_action = actionPause;
}

MVoid Player::Seek()
{
	m_action = actionSeek;
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
	m_bRun = MTrue;
	while (m_bRun)
	{
		switch (m_action)
		{
		case actionPlay:
			if (!m_context.Play())
			{
				return;
			}
			
			break;
		case actionStop:
			m_context.Stop();
			break;
		case actionPause:
			m_context.Pause();
			break;
		case actionSeek:
			if (!m_context.Seek())
			{
				return;
			}
			
			break;
		default:
			m_bRun = MFalse;
			break;
		}
	}
}

MVoid Player::thread_read()
{
	m_bRunRead = MTrue;


	MInt32 videoPercent = 0;
	MInt32 audioPercent = 0;
	AVPkt* pktHead = MNull;
	AVPkt* pktTail = MNull;
	AVPkt* pkt = MNull;
	while (m_bRunRead)
	{

		if (!m_pSourceParse->ReadFrame(&pkt))
		{
			m_bRunRead = MFalse;
			return;
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
		m_bufferPercent = videoPercent > audioPercent ? videoPercent : audioPercent;
		if (m_bufferPercent > 100)
		{
			m_bufferPercent = 100;
			MThreadSleep(m_threadHandleRead, 5);
		}

	}
}

MBool Player::prepare()
{

	//1、初始化数据读取
	if (!m_pSourceParse)
	{
		m_pSourceParse = new SourceParse();
		RETURN_FALSE(m_pSourceParse)
		RETURN_FALSE(m_pSourceParse->Open(m_strURL.Get()))

	}

	//2、初始化解码
	

	RETURN_FALSE(initDecode())

	//3、初始化播放
	if (m_pSourceParse->HasAudio())
	{
		m_audioPlay = new AudioPlayAAC();
		RETURN_FALSE(m_audioPlay)
		RETURN_FALSE(m_audioPlay->Open())

	}

	if (m_pSourceParse->HasVideo())
	{
		m_videoPlay = new VideoPlayWindow();
		RETURN_FALSE(m_videoPlay)
		m_videoPlay->SetVideoInfo(&m_videoInfo);
		RETURN_FALSE(m_videoPlay->Open())

	}

	//创建读取数据线程
	if (!m_bRunRead)
	{
		m_threadHandleRead = MThreadCreate(run_read, this);
		if (!m_threadHandleRead)
		{
			return MFalse;
		}
		MThreadResume(m_threadHandleRead);

	}

	
	m_hMutexVideo = MMutexCreate();
	m_hMutexAudio = MMutexCreate();


	m_bHasVideo = m_pSourceParse->HasVideo();
	m_bHasAudio = m_pSourceParse->HasAudio();
	return MTrue;
}


MBool	 Player::AudioDecode(MPChar buffer, MInt32& bufferSize)
{
	
	AVPkt* pktFirst = m_arrayAudio.GetNodeAndDelByIndex(1);
	m_avsync.SetCurrentAudioTime(pktFirst->pts);
	m_pFrameAudio = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
	if (!m_pFrameAudio)
	{
		return MFalse;
	}
	


	MMemCpy(buffer, m_pFrameAudio->pBuffer, m_pFrameAudio->iBufferSize);
	bufferSize = m_pFrameAudio->iBufferSize;
	m_bFirstFrameAudio = MFalse;
	//m_currentAudioTime = m_pFrameAudio->pts;
	return MTrue;

}

MBool Player::PlayOneFrame()
{
	if (!m_arrayVideo.GetSize() || !m_arrayAudio.GetSize())
	{
		m_context.SetState(Buffering);
		return MTrue;
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
			m_context.SetState(Buffering);
			return MTrue;
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
			m_context.SetState(Buffering);
			return MTrue;
		}
	}


	AVPkt*	pktVideo = m_arrayVideo.GetNodeAndDelByIndex(1);
	m_pFrameVideo = m_pDecodeVideo->DecodeFrame(pktVideo->bufferPkt, pktVideo->bufferPktSize, pktVideo->pts, pktVideo->dts);
	if (!m_pFrameVideo)
	{
		m_context.SetState(Stoping);
		return MTrue;
	}
	else if (m_pFrameVideo && !m_pFrameVideo->bSuccess)
	{
		return MTrue;
	}

	//m_currentVideoTime = m_pFrameVideo->pts;

	//if (m_currentVideoTime == m_lastVideoTime)
	//{	
	//	//表示解第一帧
	//	if (!m_audioPlay->Start())
	//	{
	//		m_context.SetState(Stoping);
	//		return MTrue;
	//	}
	//}


	//if (m_avsync.Adjust(pktVideo->pts))
	//{
	//	m_videoPlay->Display(m_pFrameVideo->pBuffer);
	//}
	m_videoPlay->Display(m_pFrameVideo->pBuffer);

	return MTrue;

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


MBool Player::buffer()
{
	if (m_bufferPercent >= 100)
	{
		return MTrue;
	}
	MThreadSleep(m_threadHandleRun,5);
	return MFalse;
}
