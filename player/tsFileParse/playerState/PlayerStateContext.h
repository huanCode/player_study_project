#ifndef _PLAYERSTATECONTEXT_H_
#define _PLAYERSTATECONTEXT_H_
#include "amcomdef.h"
#include "common.h"
enum State
{
	Prepare,
	Buffering,
	Playing,
	Stoping,
	Pauseing,
	Seeking,
	Idle,
	None
};


enum PlayerAction
{
	actionStart,
	actionPause,
	actionSeek,
	actionStop,
	actionCircle,
	actionNone
};


class Player;
class PlayerState;
class PlayerStateContext
{
public:

	PlayerStateContext();
	MBool Init(Player* pPlayer);
	MVoid SetState(State state);
	PlayerState* GetCurrentState() {
		return  m_pCurrentObject;
	};
	MBool Handle();
	MVoid Start();
	MVoid Stop();
	MVoid Pause();
	MVoid Seek(MInt64 seekTime);
	MVoid Buffer();
public:
	PlayerState		*m_pPlayerStatePrepare;
	PlayerState		*m_pPlayerStateBuffering;
	PlayerState		*m_pPlayerStatePlaying;
	PlayerState		*m_pPlayerStateStoping;
	PlayerState		*m_pPlayerStatePauseing;
	PlayerState		*m_pPlayerStateSeeking;
	PlayerState		*m_pPlayerStateIdle;
private:
	PlayerState		*m_pCurrentObject;
	PlayLock		m_playerStateLock;
};


#endif //_PLAYERSTATE_H_