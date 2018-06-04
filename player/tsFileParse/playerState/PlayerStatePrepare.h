#ifndef _PLAYERPREPARE_H_
#define _PLAYERPREPARE_H_
#include "PlayerState.h"
class PlayerStatePrepare :public PlayerState
{
public:
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();

};


#endif //_PLAYERPREPARE_H_