#ifndef _PLAYERPLAYING_H_
#define _PLAYERPLAYING_H_
#include "PlayerState.h"
class PlayerStatePlaying :public PlayerState
{
public:
	PlayerStatePlaying();
	MBool Start();
	//MVoid Buffer();
	//virtual MVoid Pause();
	//virtual MVoid Seek();

};


#endif //_PLAYERPLAYING_H_