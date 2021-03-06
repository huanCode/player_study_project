#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "amcomdef.h"
#include "amkernel.h"
#include "ToolList.h"
#include "common.h"
#include "mv3String.h"
#include "AVSync.h"
#include "PlayerStateContext.h"
#include "PlatformFactory.h"
class SourceParse;
class DecodecMgr;




class Player:public IAudioDecode
{
private:
	typedef struct _playActionData
	{
		_playActionData()
		{
			m_action = actionNone;
			seekTimeStamp = 0;
		}
		PlayerAction m_action;
		MUInt32		seekTimeStamp;

	}PlayActionData,*PtrPlayActionData;

public:
	Player();
	MBool	Start(MPChar strUrl);
	MVoid	Close();

	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek(MInt64 seekTime);
	MBool	AudioDecode(MPChar buffer, MInt32& bufferSize, MInt64* out_pts);
	MVoid SetAudioPts(MInt64 pts);

	MBool	HasVideo();
	MBool	HasAudio();
public:
	//以下函数主要在状态模式中调用
	MBool prepare();
	MInt32 buffer();
	State PlayOneFrame();
	MBool State_Seeking(MInt64 seekTime);
	MBool State_Pauseing();
	MVoid State_Stoping();
	MVoid State_Restart();	//pause-->restart

	//状态转换
	MVoid Playing_to_Buffering();
	MBool Pauseing_to_Seeking(MInt64 seekTimeStamp);
	

	//条件状态切换
	MVoid create_action(PlayerAction actionState,MBool bClear = MFalse,MInt64 seekTimeStamp = 0);



public:
	MBool initDecode();

private:
	static MDWord run(MVoid* lpPara);
	static MDWord run_read(MVoid* lpPara);
	MVoid handle();
	MVoid thread_read();


	
private:

	PlayerStateContext	m_context;
	SourceParse		*m_pSourceParse;
	//解码
	DecodecMgr		*m_pDecodeVideo;
	DecodecMgr		*m_pDecodeAudio;

	//播放
	IAudioPlay	*m_audioPlay;
	IDisplayVideo	*m_videoPlay;


	MBool			m_bRun;
	MBool			m_bExit;

	MHandle			m_threadHandleRun;

	MHandle			m_threadHandleRead;

	//video and audio array
	ToolList<AVPkt*>		m_arrayVideo;
	ToolList<AVPkt*>		m_arrayAudio;
	ToolList<PtrPlayActionData>	m_arrayPlayActionData;
	//lock 
	MHandle					m_hMutexVideo;
	MHandle					m_hMutexAudio;
	PlayLock				m_lockAction;
	PlayLock				m_lockSource;

	MInt64					m_bufferTime;
	MInt64					m_bufferPercent;


	MBool					m_bFirstFrameAudio;
	MBool					m_bFirstFrameVideo;

	Frame					*m_pFrameVideo;
	Frame					*m_pFrameAudio;

	MBool					m_bInitDecode;


	MInt64					m_currentAudioTime;
	MInt64					m_currentVideoTime;

	MInt64					m_lastAudioTime;
	MInt64					m_lastVideoTime;

	MV3String				m_strURL;

	MBool					m_bFirstVideoKeyFrame;
	MBool					m_bFirstAudioKeyFrame;

	MBool					m_bHasVideo;
	MBool					m_bHasAudio;


	AVSync					m_avsync;

	VideoInfo				m_videoInfo;

	MInt64					m_time;


	MBool					m_bSeek;

	MBool					m_bForbidRead;

};


#endif //_PLAYER_H_