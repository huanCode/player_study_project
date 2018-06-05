#ifndef _PLAYERSEEKING_H_
#define _PLAYERSEEKING_H_
#include "PlayerState.h"
class PlayerStateSeeking :public PlayerState
{
public:
	virtual MBool Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();

};


#endif //_PLAYERSEEKING_H_