#include "stdafx.h"
#include "PlayerStateBuffering.h"


MBool PlayerStateBuffering::Play()
{
	if (m_pPlayer->buffer())
	{
		m_stateContext->SetState(State::Playing);
	}

	return MTrue;
}

MVoid PlayerStateBuffering::Stop()
{

}

MVoid PlayerStateBuffering::Pause()
{

}

MBool PlayerStateBuffering::Seek()
{
	return MTrue;
}
