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
}

MBool PlayerStateBuffering::Start()
{
	if (m_pPlayer->buffer())
	{
		m_stateContext->SetState(State::Playing);
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
