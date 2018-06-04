#ifndef _PLAYERSEEKING_H_
#define _PLAYERSEEKING_H_
#include "PlayerState.h"
class PlayerStateSeeking :public PlayerState
{
public:
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();

};


#endif //_PLAYERSEEKING_H_