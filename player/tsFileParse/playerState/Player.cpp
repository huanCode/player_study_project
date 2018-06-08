#include "stdafx.h"
#include "Player.h"
#include "SourceParse.h"
#include "AudioPlayAAC.h"
#include "VideoPlayWindow.h"
#include "DecodeH264.h"
#include "DecodeAAC.h"
#define BUFFER_TIME		5000  //5s

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





MBool Player::Play(MPChar strUrl)
{
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
			m_context.Play();
			break;
		case actionStop:
			m_context.Stop();
			break;
		case actionPause:
			m_context.Pause();
			break;
		case actionSeek:
			m_context.Seek();
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
	while (m_bRunRead)
	{
		if (m_arrayVideo.GetSize() > 2)
		{
			AVPkt* pktFirst = m_arrayVideo.GetFirstNode();
			AVPkt* pktLast = m_arrayVideo.GetLastNode();

			MInt64 duration = pktFirst->pts - pktLast->pts;
			if (duration > m_bufferTime)
			{
				MThreadSleep(m_threadHandleRead,50);
			}
			else
			{
				MInt64 minDuration = duration > m_bufferTime ? m_bufferTime : duration;
				m_bufferPercent = (minDuration * 100) / m_bufferTime;
			}

		}




		AVPkt* pkt = MNull;
		
		if (!m_pSourceParse->ReadFrame(&pkt))
		{
			m_bRunRead = MFalse;
			return;
		}

		//AVPkt* pkt_tmp = new AVPkt();
		//pkt_tmp->Init();
		//pkt_tmp->DepthCopy(pkt);
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
	}
}

MBool Player::prepare()
{


	m_audioPlay = new AudioPlayAAC();
	if (!m_audioPlay)
	{
		return MFalse;
	}
	if (!m_audioPlay->Open())
	{
		return MFalse;
	}

	m_videoPlay = new VideoPlayWindow();
	if (!m_videoPlay)
	{
		return MFalse;
	}
	if (!m_videoPlay->Open())
	{
		return MFalse;
	}

	if (!m_pSourceParse)
	{
		m_pSourceParse = new SourceParse();
		if (!m_pSourceParse)
		{
			return MFalse;
		}
		MPChar strUrl = "http://38uni317.vod2.hongshiyun.net/target/hls/2017/01/13/660_3deb5b1a9a3f4f3d9c49f52f41a19a60_10_1280x720.m3u8";
		
		if (!m_pSourceParse->Open(strUrl))
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
	if (m_bInitDecode)
	{
		return MTrue;
	}


	if (!m_pDecodeVideo)
	{
		AVPkt* pktFirst = m_arrayVideo.GetFirstNode();
		if (pktFirst->codeType == _AV_CODEC_ID_H264)
		{
			m_pDecodeVideo = new DecodeH264();
			if (!m_pDecodeVideo)
			{
				return MFalse;
			}
		}
		
	}


	if (!m_pDecodeAudio)
	{
		AVPkt* pktFirst = m_arrayAudio.GetFirstNode();
		if (pktFirst->codeType == _AV_CODEC_ID_AAC)
		{
			m_pDecodeAudio = new DecodeAAC();
			if (!m_pDecodeAudio)
			{
				return MFalse;
			}
		}

	}


	if (m_bFirstFrameVideo)
	{
		AVPkt* pktFirst = MNull;
		MInt32 count = 20;
		while (count-- && m_arrayVideo.GetSize())
		{
			pktFirst = m_arrayVideo.GetNodeAndDelByIndex(1);
			m_pFrameVideo = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
			if (!m_pFrameAudio)
			{
				return MFalse;
			}
			else if (m_pFrameVideo->bSuccess)
			{
				
				m_currentVideoTime = m_pFrameVideo->pts;
				m_bFirstFrameVideo = MFalse;
			}
		}

		
		
	}


	if (m_bFirstFrameAudio)
	{
		//AVPkt* pktFirst = m_arrayAudio.GetNodeAndDelByIndex(1);
		//m_pFrameAudio = m_pDecodeAudio->DecodeFrame(pktFirst->bufferPkt, pktFirst->bufferPktSize, pktFirst->pts, pktFirst->dts);
		//if (!m_pFrameAudio)
		//{
		//	return MFalse;
		//}

		m_audioPlay->SetPlayer(this);
		
		if (!m_audioPlay->Start())
		{
			return MFalse;
		}
		MInt32 count = 20;
		while (count--)
		{
			if (m_bFirstFrameAudio)
			{
				MThreadSleep(m_threadHandleRun, 2);
			}
			else
			{
				m_bInitDecode = MTrue;
				return MTrue;
			}
			
		}

	}



	return MFalse;

}


MBool Player::buffer()
{
	if (m_bufferPercent == 100)
	{
		return MTrue;
	}
	MThreadSleep(m_threadHandleRun,40);
	return MFalse;
}
