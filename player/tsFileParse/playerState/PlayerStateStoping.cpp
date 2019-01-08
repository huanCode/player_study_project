#include "stdafx.h"
#include "PlayerStateStoping.h"

PlayerStateStoping::PlayerStateStoping()
{
	m_strState = "Stoping";

	m_stateRight[actionStart] = MTrue;
	m_stateRight[actionPause] = MFalse;
	m_stateRight[actionSeek] = MFalse;
	m_stateRight[actionStop] = MFalse;
	m_stateRight[actionCircle] = MFalse;

	m_lastState = Stoping;
}

MBool PlayerStateStoping::Start()
{
	return MTrue;
}

MVoid PlayerStateStoping::Stop()
{

}

MVoid PlayerStateStoping::Pause()
{

}

MBool PlayerStateStoping::Seek()
{
	return MTrue;
}
