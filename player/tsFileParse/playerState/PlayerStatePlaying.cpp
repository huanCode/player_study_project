#include "stdafx.h"
#include "PlayerStatePlaying.h"

MBool PlayerStatePlaying::Play()
{
	return m_pPlayer->PlayOneFrame();
	//m_stateContext->SetState(State::Playing);
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
