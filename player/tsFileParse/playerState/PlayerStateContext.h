#ifndef _PLAYERSTATECONTEXT_H_
#define _PLAYERSTATECONTEXT_H_
#include "amcomdef.h"
class PlayerState;
class PlayerStateContext
{
public:

	PlayerStateContext();
	MBool Init();
	MVoid SetState(State state);
	PlayerState* GetCurrentState() {
		return  m_pCurrentObject;
	};
	MVoid Play();
	MVoid Stop();
	MVoid Pause();
	MVoid Seek();
public:
	PlayerState		*m_pPlayerStatePrepare;
	PlayerState		*m_pPlayerStateBuffering;
	PlayerState		*m_pPlayerStatePlaying;
	PlayerState		*m_pPlayerStateStoping;
	PlayerState		*m_pPlayerStatePauseing;
	PlayerState		*m_pPlayerStateSeeking;

private:
	PlayerState		*m_pCurrentObject;

};


#endif //_PLAYERSTATE_H_