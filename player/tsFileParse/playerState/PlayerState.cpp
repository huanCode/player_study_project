#include "stdafx.h"
#include "PlayerState.h"

PlayerState::PlayerState()
{
	m_pPlayer = MNull;
	m_stateContext = MNull;
}

MVoid PlayerState::SetContext(PlayerStateContext *pContext)
{
	m_stateContext = pContext;
}

MVoid PlayerState::SetPlayer(Player* pPlayer)
{
	m_pPlayer = pPlayer;
}

MBool PlayerState::Play()
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

MBool PlayerState::Seek()
{
	m_stateContext->SetState(State::Seeking);
	return m_stateContext->GetCurrentState()->Seek();


}