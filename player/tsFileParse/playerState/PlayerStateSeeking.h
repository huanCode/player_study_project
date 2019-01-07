#ifndef _PLAYERSEEKING_H_
#define _PLAYERSEEKING_H_
#include "PlayerState.h"
class PlayerStateSeeking :public PlayerState
{
public:
	PlayerStateSeeking()
	{
		m_seekTimeStamp = 0;
	}
	virtual MBool Play();
	virtual MVoid Stop();
	virtual MVoid Pause();
	virtual MBool Seek();


	MVoid SetSeekTimeStamp(MInt64 seekTime)
	{
		m_seekTimeStamp = seekTime;
	}
private:
	MInt64	m_seekTimeStamp;
};


#endif //_PLAYERSEEKING_H_