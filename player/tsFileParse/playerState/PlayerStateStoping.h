#ifndef _PLAYERSTOPING_H_
#define _PLAYERSTOPING_H_
#include "PlayerState.h"
class PlayerStateStoping :public PlayerState
{
public:
	PlayerStateStoping();
	//virtual MBool Start();
	virtual MVoid Stop();
	//virtual MVoid Pause();
	//virtual MBool Seek();

};


#endif //_PLAYERSTOPING_H_