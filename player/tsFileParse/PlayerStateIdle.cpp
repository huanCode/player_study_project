#include "PlayerStateIdle.h"
#include <windows.h>
#define DEFAULT_TIME_MS	5;

MBool PlayerStateIdle::Handle()
{
	Sleep(5);
	return MTrue;
}