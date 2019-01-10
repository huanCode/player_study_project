#ifndef _PLAYERSTATEBUFFERING_H_
#define _PLAYERSTATEBUFFERING_H_
#include "PlayerState.h"
class PlayerStateBuffering :public PlayerState
{
public:
	PlayerStateBuffering();
	virtual MBool Start();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();

private:
	MBool	m_bFirstBuffer;

};


#endif //_PLAYERPAUSEING_H_