#include "stdafx.h"
#include "PlayerStatePlaying.h"

PlayerStatePlaying::PlayerStatePlaying()
{
	m_strState = "Playing";

	m_stateRight[actionStart] = MFalse;
	m_stateRight[actionPause] = MTrue;
	m_stateRight[actionSeek] = MTrue;
	m_stateRight[actionStop] = MTrue;
	m_stateRight[actionCircle] = MTrue;

	m_lastState = Playing;
	m_currentState = State::Playing;
}

MBool PlayerStatePlaying::Start()
{
	State ret = m_pPlayer->PlayOneFrame();
	if (ret == State::Buffering)
	{
		m_stateContext->SetState(State::Buffering);
	}
	else if (ret == State::Stoping)
	{
		m_pPlayer->create_action(PlayerAction::actionStop,MTrue);
	}
	
	return MTrue;
}

//MVoid PlayerStatePlaying::Buffer()
//{
//	m_pPlayer->Playing_to_Buffering();
//	m_stateContext->SetState(State::Buffering);
//}
////
////MVoid PlayerStatePlaying::Pause()
////{
////
////}
////
//MBool PlayerStatePlaying::Seek(MInt64 seekTimeStamp)
//{
//	m_stateContext->SetState(State::Seeking);
//
//	return MTrue;
//}
