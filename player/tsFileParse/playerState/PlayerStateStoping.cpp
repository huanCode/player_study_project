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
	m_currentState = State::Stoping;
}



MVoid PlayerStateStoping::Stop()
{
	m_pPlayer->State_Stoping();
}

