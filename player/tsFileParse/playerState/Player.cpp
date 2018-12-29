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
}

MVoid Player::Start()
{

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




MBool Player::Play(MPChar strUrl)
{
	if (!strUrl)
	{
		return MFalse;
	}

	m_strURL.StrCat(strUrl);

	m_context.Init(this);

	m_action = actionPlay;
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
			;
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

	
	MBool hasVideo = m_pSourceParse->HasVideo();
	MBool hasAudio = m_pSourceParse->HasAudio();
	MInt32 videoPercent = 0;
	MInt32 audioPercent = 0;
	AVPkt* pktFirst = MNull;
	AVPkt* pktLast = MNull;
	while (m_bRunRead)
	{





		AVPkt* pkt = MNull;
		
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


		if (hasVideo)
		{
			videoPercent = 0;
			MMutexLock(m_hMutexVideo);
			if (m_arrayVideo.GetSize() > 2)
			{
				pktFirst = m_arrayVideo.GetFirstNode();
				pktLast = m_arrayVideo.GetLastNode();

				MInt64 duration = pktFirst->pts - pktLast->pts;
				//MInt64 minDuration = duration > m_bufferTime ? m_bufferTime : duration;
				videoPercent = (duration * 100) / m_bufferTime;
				//if (m_bufferPercent > 100)
				//{
				//	m_bufferPercent = 100;
				//	MThreadSleep(m_threadHandleRead, 5);
				//}



			}
			MMutexUnlock(m_hMutexVideo);
		}

		if (hasAudio)
		{
			audioPercent = 0;
			MMutexLock(m_hMutexAudio);
			if (m_arrayAudio.GetSize() > 2)
			{
				pktFirst = m_arrayAudio.GetFirstNode();
				pktLast = m_arrayAudio.GetLastNode();

				MInt64 duration = pktFirst->pts - pktLast->pts;
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
		RETURN_BOOL(!m_pSourceParse)
		RETURN_BOOL(m_pSourceParse->Open(m_strURL.Get()))

	}

	//2、初始化解码
	

	RETURN_BOOL(initDecode())

	//3、初始化播放
	if (m_pSourceParse->HasAudio())
	{
		m_audioPlay = new AudioPlayAAC();
		RETURN_BOOL(!m_audioPlay)

		if (!m_audioPlay->Open())
		{
			return MFalse;
		}
	}

	if (m_pSourceParse->HasVideo())
	{
		m_videoPlay = new VideoPlayWindow();
		if (!m_videoPlay)
		{
			return MFalse;
		}
		if (!m_videoPlay->Open())
		{
			return MFalse;
		}
	}







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

	return MTrue;
}


MBool	 Player::AudioDecode(MPChar buffer, MInt32& bufferSize)
{
	AVPkt* pktFirst = m_arrayAudio.GetNodeAndDelByIndex(1);
	m_pFrameAudio = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
	if (!m_pFrameAudio)
	{
		return MFalse;
	}
	
	MMemCpy(buffer, m_pFrameAudio->pBuffer, m_pFrameAudio->iBufferSize);
	bufferSize = m_pFrameAudio->iBufferSize;
	m_bFirstFrameAudio = MFalse;
	m_currentAudioTime = m_pFrameAudio->pts;
	return MTrue;

}

MBool Player::PlayOneFrame()
{
	if (!m_arrayVideo.GetSize() || !m_arrayAudio.GetSize())
	{
		m_context.SetState(Buffering);
		return MTrue;
	}

	if (!initDecode())
	{
		return MFalse;
	}

	//if (true)
	//{

	//}
	m_videoPlay->Display(m_pFrameVideo->pBuffer);

	MThreadSleep(m_threadHandleRun, 40);

}

MBool Player::initDecode()
{

	if (m_pSourceParse->HasVideo())
	{
		RETURN_BOOL(m_pDecodeVideo->Open(_AV_CODEC_ID_H264))
	}

	if (m_pSourceParse->HasAudio())
	{
		RETURN_BOOL(m_pDecodeAudio->Open(_AV_CODEC_ID_AAC))
	}

	return MTrue;




	//if (m_bFirstFrameVideo)
	//{
	//	AVPkt* pktFirst = MNull;
	//	MInt32 count = 20;
	//	while (count-- && m_arrayVideo.GetSize())
	//	{
	//		pktFirst = m_arrayVideo.GetNodeAndDelByIndex(1);
	//		m_pFrameVideo = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
	//		if (!m_pFrameAudio)
	//		{
	//			return MFalse;
	//		}
	//		else if (m_pFrameVideo->bSuccess)
	//		{
	//			m_currentVideoTime = m_pFrameVideo->pts;
	//			m_bFirstFrameVideo = MFalse;
	//		}
	//	}
	//}


	//if (m_bFirstFrameAudio)
	//{
		//AVPkt* pktFirst = m_arrayAudio.GetNodeAndDelByIndex(1);
		//m_pFrameAudio = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
		//if (!m_pFrameAudio)
		//{
		//	return MFalse;
		//}

	//	m_audioPlay->SetPlayer(this);
	//	
	//	if (!m_audioPlay->Start())
	//	{
	//		return MFalse;
	//	}
	//	MInt32 count = 20;
	//	while (count--)
	//	{
	//		if (m_bFirstFrameAudio)
	//		{
	//			MThreadSleep(m_threadHandleRun, 2);
	//		}
	//		else
	//		{
	//			m_bInitDecode = MTrue;
	//			return MTrue;
	//		}
	//		
	//	}

	//}


	return MFalse;

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
