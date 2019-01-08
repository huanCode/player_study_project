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
}

MBool PlayerStatePlaying::Start()
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
