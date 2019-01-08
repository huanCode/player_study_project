#include "stdafx.h"
#include "PlayerStatePauseing.h"

PlayerStatePauseing::PlayerStatePauseing()
{
	m_strState = "Pauseing";

	m_stateRight[actionStart] = MTrue;
	m_stateRight[actionPause] = MFalse;
	m_stateRight[actionSeek] = MTrue;
	m_stateRight[actionStop] = MTrue;
	m_stateRight[actionCircle] = MFalse;

	m_lastState = Pauseing;
}

MBool PlayerStatePauseing::Start()
{
	return MTrue;

}

MVoid PlayerStatePauseing::Stop()
{

}

MVoid PlayerStatePauseing::Pause()
{
	m_pPlayer->State_Pauseing();
}

MBool PlayerStatePauseing::Seek()
{
	return MTrue;
}
