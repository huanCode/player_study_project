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
	virtual MBool Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();

public:
	Player*				m_pPlayer;
	PlayerStateContext	*m_stateContext;
};


#endif //_PLAYERSTATE_H_