#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "amcomdef.h"
class SourceParse;
class IDecode;
class VideoPlayWindow;
class AudioPlayAAC;
class Player
{
public:
	Player();
	MVoid	Start();
	MVoid	Close();
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();


private:
	//½âÂë
	IDecode		*m_pDecodeVideo;
	IDecode		*m_pDecodeAudio;

	//²¥·Å
	AudioPlayAAC	*m_audioPlay;
	VideoPlayWindow	*m_videoPlay;
};


#endif //_PLAYER_H_