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


State PlayerState::GetCurrentState()
{
	return m_currentState;
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
	m_stateContext->Start();
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
	m_stateContext->SetState(State::Seeking);
	//PlayerStateSeeking*  seekObj = (PlayerStateSeeking*)m_pCurrentObject;
	if (m_stateContext->GetCurrentState()->Seek(seekTimeStamp))
	{
		if (m_lastState == State::Pauseing)
		{
			if (m_pPlayer->Pauseing_to_Seeking(seekTimeStamp))
			{
				m_stateContext->SetState(State::Pauseing);
				m_pPlayer->create_action(PlayerAction::actionPause);
			}
			else
			{
				m_pPlayer->create_action(PlayerAction::actionStop);
				return MFalse;
			}
			//m_stateContext->SetState(State::Pauseing);
			
		}
		else
		{
			m_stateContext->SetState(State::Buffering);
			m_pPlayer->create_action(PlayerAction::actionStart);
			
		}
		

		return MTrue;
	}
	m_pPlayer->create_action(PlayerAction::actionStop);
	return MFalse;
}

MVoid PlayerState::Buffer()
{
	m_stateContext->SetState(Buffering);
	m_stateContext->GetCurrentState()->Buffer();

}