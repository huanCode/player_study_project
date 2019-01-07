#include "stdafx.h"
#include "PlayerStatePrepare.h"


MBool PlayerStatePrepare::Handle()
{
	if (m_pPlayer->prepare())
	{
		m_stateContext->SetState(Buffering);
		return MTrue;
	}

	return MFalse;
}

MBool PlayerStatePrepare::Play()
{
	//
	//if (m_pPlayer->prepare())
	//{
	//	m_stateContext->SetState(Buffering);
	//	return MTrue;
	//}
	
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
