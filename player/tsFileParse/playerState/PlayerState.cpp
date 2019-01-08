#include "stdafx.h"
#include "PlayerState.h"

PlayerState::PlayerState()
{
	m_pPlayer = MNull;
	m_stateContext = MNull;
	m_strState = MNull;

	MMemSet(m_stateRight,0,sizeof(m_stateRight));
	m_lastState = None;
}


MVoid PlayerState::SetLastState(State s)
{
	m_lastState = s;
}


MBool PlayerState::QueryRight(PlayerAction action)
{
	return m_stateRight[action];
}

MVoid PlayerState::SetContext(PlayerStateContext *pContext)
{
	m_stateContext = pContext;
}

MVoid PlayerState::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

MBool PlayerState::Handle()
{
	return MTrue;
}

MBool PlayerState::Start()
{
	//m_stateContext->SetState(State::Playing);
	return MTrue;
}

MVoid PlayerState::Stop()
{
	m_stateContext->SetState(State::Stoping);
	m_stateContext->GetCurrentState()->Stop();
}

MVoid PlayerState::Pause()
{
	m_stateContext->SetState(State::Pauseing);
	m_stateContext->GetCurrentState()->Pause();
}

MBool PlayerState::Seek(MInt64 seekTimeStamp)
{
	m_stateContext->SetState(Seeking);
	//PlayerStateSeeking*  seekObj = (PlayerStateSeeking*)m_pCurrentObject;
	if (m_stateContext->GetCurrentState()->Seek(seekTimeStamp))
	{
		m_stateContext->SetState(Buffering);
		return MTrue;
	}
	return MFalse;
}

MInt32 PlayerState::Buffer()
{
	m_stateContext->SetState(Buffering);
	if (m_stateContext->GetCurrentState()->B)
	{

	}

}