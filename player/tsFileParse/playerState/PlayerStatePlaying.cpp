#include "stdafx.h"
#include "PlayerStatePlaying.h"

PlayerStatePlaying::PlayerStatePlaying()
{
	m_strState = "Playing";
}

MBool PlayerStatePlaying::Play()
{
	
	if (!m_pPlayer->PlayOneFrame())
	{
		m_stateContext->SetState(State::Stoping);
	}
	
	return MTrue;
}

MVoid PlayerStatePlaying::Buffer()
{
	m_pPlayer->Playing_to_Buffering();
	m_stateContext->SetState(State::Buffering);
}
//
//MVoid PlayerStatePlaying::Pause()
//{
//
//}
//
MBool PlayerStatePlaying::Seek(MInt64 seekTimeStamp)
{
	m_stateContext->SetState(State::Seeking);

	return MTrue;
}
