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
	MPChar GetStateName()
	{
		return m_strState;
	}
	virtual MBool Handle();
	virtual MBool Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek(MInt64 seekTimeStamp);
	virtual MVoid Buffer();
public:
	Player*				m_pPlayer;
	PlayerStateContext	*m_stateContext;
	MPChar				m_strState;
};


#endif //_PLAYERSTATE_H_