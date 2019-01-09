#ifndef _PLAYERSTATE_H_
#define _PLAYERSTATE_H_
#include "amcomdef.h"



#include "Player.h"
#include "PlayerStateContext.h"

//class Player;
class PlayerState
{
public:
	PlayerState();
	//virtual MVoid Idle() =0;
	MVoid SetContext(PlayerStateContext *pContext);
	MVoid SetPlayer(Player* pPlayer);
	MVoid SetLastState(State s);
	MBool QueryRight(PlayerAction action);
	State GetCurrentState();
	MPChar GetStateName()
	{
		return m_strState;
	}
	virtual MBool Handle();
	virtual MBool Start();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek(MInt64 seekTimeStamp);
	virtual MVoid Buffer();
public:
	Player*				m_pPlayer;
	PlayerStateContext	*m_stateContext;
	MPChar				m_strState;
	MBool				m_stateRight[5];

	State				m_lastState;
	State				m_currentState;
};


#endif //_PLAYERSTATE_H_