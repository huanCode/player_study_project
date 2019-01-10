#include "stdafx.h"
#include "PlayerStatePauseing.h"
#include <windows.h>
PlayerStatePauseing::PlayerStatePauseing()
{
	m_strState = "Pauseing";

	m_stateRight[actionStart] = MTrue;
	m_stateRight[actionPause] = MFalse;
	m_stateRight[actionSeek] = MTrue;
	m_stateRight[actionStop] = MTrue;
	m_stateRight[actionCircle] = MFalse;

	m_lastState = Pauseing;
	m_currentState = State::Pauseing;
	m_bPause = MFalse;
}

MBool PlayerStatePauseing::Start()
{
	m_pPlayer->State_Restart();
	if (m_lastState == State::Prepare || m_lastState == State::Seeking || m_lastState == State::Buffering)
	{
		m_stateContext->SetState(State::Buffering);
	}
	else if (m_lastState == State::Playing || m_lastState == State::Pauseing)
	{
		m_stateContext->SetState(State::Playing);
	}

	return MTrue;
}

MVoid PlayerStatePauseing::Stop()
{

}

MVoid PlayerStatePauseing::Pause()
{
	if (m_bPause)
	{
		Sleep(20);
	}
	else
	{
		m_pPlayer->State_Pauseing();
		m_bPause = MTrue;
	}
	
}

MBool PlayerStatePauseing::Seek()
{
	return MTrue;
}
