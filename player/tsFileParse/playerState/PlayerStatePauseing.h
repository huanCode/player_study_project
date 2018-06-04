#ifndef _PLAYERPAUSEING_H_
#define _PLAYERPAUSEING_H_
#include "PlayerState.h"
class PlayerStatePauseing:public PlayerState
{
public:
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();

};


#endif //_PLAYERPAUSEING_H_