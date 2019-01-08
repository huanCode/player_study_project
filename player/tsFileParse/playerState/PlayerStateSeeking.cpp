#include "stdafx.h"
#include "PlayerStateSeeking.h"


PlayerStateSeeking::PlayerStateSeeking()
{
	m_seekTimeStamp = 0;
	m_strState = "Seeking";

	m_stateRight[actionStart] = MFalse;
	m_stateRight[actionPause] = MTrue;
	m_stateRight[actionSeek] = MTrue;
	m_stateRight[actionStop] = MTrue;
	m_stateRight[actionCircle] = MFalse;

	m_lastState = Seeking;
}

MBool PlayerStateSeeking::Start()
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
