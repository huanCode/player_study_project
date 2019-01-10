#include "stdafx.h"
#include "PlayerStateBuffering.h"

PlayerStateBuffering::PlayerStateBuffering()
{
	m_strState = "Buffering";

	m_stateRight[actionStart] = MFalse;
	m_stateRight[actionPause] = MTrue;
	m_stateRight[actionSeek] = MTrue;
	m_stateRight[actionStop] = MTrue;
	m_stateRight[actionCircle] = MTrue;

	m_lastState = Buffering;
	m_currentState = State::Buffering;

	m_bFirstBuffer = MTrue;
}

MBool PlayerStateBuffering::Start()
{
	if (m_bFirstBuffer)
	{
		m_pPlayer->State_Pauseing();
		m_bFirstBuffer = MFalse;
	}
	if (m_pPlayer->buffer() >= 100)
	{
		m_stateContext->SetState(State::Playing);
		if (!m_bFirstBuffer)
		{
			m_pPlayer->State_Restart();
			m_bFirstBuffer = MTrue;
		}
	}

	return MTrue;
}

MVoid PlayerStateBuffering::Stop()
{

}

MVoid PlayerStateBuffering::Pause()
{

}

MBool PlayerStateBuffering::Seek()
{
	return MTrue;
}
