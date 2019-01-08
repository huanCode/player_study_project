#include "PlayerStateIdle.h"
#include <windows.h>
#define DEFAULT_TIME_MS	5;

PlayerStateIdle::PlayerStateIdle()
{
	m_strState = "Idleing";
}

MBool PlayerStateIdle::Handle()
{
	Sleep(5);
	return MTrue;
}