#include "stdafx.h"
#include "PlayerStatePrepare.h"


MBool PlayerStatePrepare::Play()
{
	//
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
