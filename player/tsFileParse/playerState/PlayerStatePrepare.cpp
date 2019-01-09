#include "stdafx.h"
#include "PlayerStatePrepare.h"


PlayerStatePrepare::PlayerStatePrepare()
{
	m_strState = "Prepareing";
	m_stateRight[actionStart]	= MTrue;
	m_stateRight[actionPause]	= MTrue;
	m_stateRight[actionSeek]	= MTrue;
	m_stateRight[actionStop]	= MTrue;
	m_stateRight[actionCircle]	= MFalse;

	m_lastState = Prepare;

	m_currentState = State::Prepare;
}


MBool PlayerStatePrepare::Start()
{
	if (m_pPlayer->prepare())
	{
		m_stateContext->SetState(Buffering);
		return MTrue;
	}

	return MFalse;
}

//MVoid PlayerStatePrepare::Stop()
//{
//
//}
//
//MVoid PlayerStatePrepare::Pause()
//{
//
//}
//
//MVoid PlayerStatePrepare::Seek()
//{
//
//}
