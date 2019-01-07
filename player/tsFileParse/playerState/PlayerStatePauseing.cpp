#include "stdafx.h"
#include "PlayerStatePauseing.h"

PlayerStatePauseing::PlayerStatePauseing()
{
}

MBool PlayerStatePauseing::Play()
{
	return MTrue;

}

MVoid PlayerStatePauseing::Stop()
{

}

MVoid PlayerStatePauseing::Pause()
{
	m_pPlayer->State_Pauseing();
}

MBool PlayerStatePauseing::Seek()
{
	return MTrue;
}
