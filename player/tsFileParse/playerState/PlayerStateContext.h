#ifndef _PLAYERSTATECONTEXT_H_
#define _PLAYERSTATECONTEXT_H_
#include "amcomdef.h"

enum State
{
	Prepare,
	Buffering,
	Playing,
	Stoping,
	Pauseing,
	Seeking
};


class Player;
class PlayerState;
class PlayerStateContext
{
public:

	PlayerStateContext();
	MBool Init(Player* pPlayer);
	MVoid SetState(State state);
	PlayerState* GetCurrentState() {
		return  m_pCurrentObject;
	};
	MBool Play();
	MVoid Stop();
	MVoid Pause();
	MBool Seek();
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