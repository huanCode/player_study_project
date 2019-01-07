#ifndef _PLAYERSTATEIDLE_H_
#define _PLAYERSTATEIDLE_H_
#include "playerState.h"
class PlayerStateIdle:public PlayerState
{
public:
	PlayerStateIdle();

	MBool Handle();
};

#endif //_PLAYERSTATEIDLE_H_