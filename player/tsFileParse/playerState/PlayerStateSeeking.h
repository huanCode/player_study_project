#ifndef _PLAYERSEEKING_H_
#define _PLAYERSEEKING_H_
#include "PlayerState.h"
class PlayerStateSeeking :public PlayerState
{
public:
	PlayerStateSeeking();
	//virtual MBool Start();
	//virtual MVoid Stop();
	//virtual MVoid Pause();
	virtual MBool Seek(MInt64 seekTimeStamp);


	MVoid SetSeekTimeStamp(MInt64 seekTime)
	{
		m_seekTimeStamp = seekTime;
	}
private:
	MInt64	m_seekTimeStamp;
};


#endif //_PLAYERSEEKING_H_