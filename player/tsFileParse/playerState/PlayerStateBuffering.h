#ifndef _PLAYERSTATEBUFFERING_H_
#define _PLAYERSTATEBUFFERING_H_
#include "PlayerState.h"
class PlayerStateBuffering :public PlayerState
{
public:
	virtual MBool Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();

};


#endif //_PLAYERPAUSEING_H_