#include "stdafx.h"
#include "PlayerStateSeeking.h"


MBool PlayerStateSeeking::Play()
{
	return MTrue;
}

MVoid PlayerStateSeeking::Stop()
{

}

MVoid PlayerStateSeeking::Pause()
{

}

MBool PlayerStateSeeking::Seek(MInt64 seekTimeStamp)
{
	return m_pPlayer->State_Seeking(seekTimeStamp);
}
