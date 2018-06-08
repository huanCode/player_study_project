#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "amcomdef.h"
#include "MKernel.h"
#include "PlayerStateContext.h"
#include "ToolList.h"
#include "common.h"
class SourceParse;
class IDecode;
class VideoPlayWindow;
class AudioPlayAAC;

enum PlayerAction
{
	actionPlay,
	actionStop,
	actionPause,
	actionSeek
};


class Player
{
public:
	Player();
	MVoid	Start();
	MVoid	Close();
	virtual MBool Play(MPChar strUrl);
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();
	MBool	AudioDecode(MPChar buffer,MInt32& bufferSize);
public:
	//以下函数主要在状态模式中调用
	MBool prepare();
	MBool buffer();
	MBool PlayOneFrame();
private:
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
	IDecode		*m_pDecodeVideo;
	IDecode		*m_pDecodeAudio;

	//播放
	AudioPlayAAC	*m_audioPlay;
	VideoPlayWindow	*m_videoPlay;


	MBool			m_bRun;
	MBool			m_bRunRead;
	PlayerAction	m_action;
	MHandle			m_threadHandleRun;

	MHandle			m_threadHandleRead;

	//video and audio array
	ToolList<AVPkt*>		m_arrayVideo;
	ToolList<AVPkt*>		m_arrayAudio;

	//lock 
	MHandle					m_hMutexVideo;
	MHandle					m_hMutexAudio;
	MInt64					m_bufferTime;
	MInt64					m_bufferPercent;


	MBool					m_bFirstFrameAudio;
	MBool					m_bFirstFrameVideo;

	Frame					*m_pFrameVideo;
	Frame					*m_pFrameAudio;

	MBool					m_bInitDecode;


	MInt64					m_currentAudioTime;
	MInt64					m_currentVideoTime;
};


#endif //_PLAYER_H_