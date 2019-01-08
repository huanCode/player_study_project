#ifndef _PLAYERPAUSEING_H_
#define _PLAYERPAUSEING_H_
#include "PlayerState.h"
class PlayerStatePauseing:public PlayerState
{
public:
	PlayerStatePauseing();
	virtual MBool Start();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();

};


#endif //_PLAYERPAUSEING_H_