#include "PlayerState.h"

PlayerState::PlayerState()
{

}

MVoid PlayerState::SetContext(PlayerStateContext *pContext)
{
	m_stateContext = pContext;
}

MVoid PlayerState::Play()
{
	//m_stateContext->SetState(State::Playing);
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

MVoid PlayerState::Seek()
{
	m_stateContext->SetState(State::Seeking);
	m_stateContext->GetCurrentState()->Seek();
}