#include "stdafx.h"
#include "PlayerStatePlaying.h"

MBool PlayerStatePlaying::Play()
{
	
	if (!m_pPlayer->PlayOneFrame())
	{
		m_stateContext->SetState(State::Stoping);
	}
	
	return MTrue;
}

//MVoid PlayerStatePlaying::Stop()
//{
//	m_stateContext->SetState(State::);
//}
//
//MVoid PlayerStatePlaying::Pause()
//{
//
//}
//
//MVoid PlayerStatePlaying::Seek()
//{
//
//}
