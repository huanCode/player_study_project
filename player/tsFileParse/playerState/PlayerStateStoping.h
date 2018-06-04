#ifndef _PLAYERSTOPING_H_
#define _PLAYERSTOPING_H_
#include "PlayerState.h"
class PlayerStateStoping :public PlayerState
{
public:
	virtual MVoid Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MVoid Seek();

};


#endif //_PLAYERSTOPING_H_