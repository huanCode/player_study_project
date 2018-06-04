#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_
#include "amcomdef.h"
#include "PlayerStateContext.h"
enum State
{
	Prepare,
	Buffering,
	Playing,
	Stoping,
	Pauseing,
	Seeking
};

class PlayerState
{
public:
	PlayerState();
	//virtual MVoid Idle() =0;
	MVoid SetContext(PlayerStateContext *pContext);
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();

public:
	PlayerStateContext	*m_stateContext;
};


#endif //_PLAYERSTATE_H_