#include "stdafx.h"
#include "PlayerStateContext.h"
#include "PlayerStatePrepare.h"
#include "PlayerStateBuffering.h"
#include "PlayerStatePlaying.h"
#include "PlayerStateStoping.h"
#include "PlayerStatePauseing.h"
#include "PlayerStateSeeking.h"

#define CREATE_STATE(object,class_name,pPlayer) 	if (!object) \
						{	\
							object = new class_name();	\
							if (!object)	\
								return MFalse; \
							object->SetContext(this); \
							object->SetPlayer(pPlayer); \
						}	\

PlayerStateContext::PlayerStateContext()
{
	m_pPlayerStatePrepare		= MNull;
	m_pPlayerStateBuffering		= MNull;
	m_pPlayerStatePlaying		= MNull;
	m_pPlayerStateStoping		= MNull;
	m_pPlayerStatePauseing		= MNull;
	m_pPlayerStateSeeking		= MNull;
	m_pCurrentObject			= MNull;
}

MBool PlayerStateContext::Init(Player* pPlayer)
{
	if (!pPlayer)
	{
		return MFalse;
	}

	
	RETURN_FALSE(m_playerStateLock.Init())
	//if (!m_pPlayerStatePrepare)
	//{
	//	m_pPlayerStatePrepare = new PlayerStatePrepare();
	//	if (!m_pPlayerStatePrepare)
	//	{
	//		return MFalse;
	//	}
	//}
	CREATE_STATE(m_pPlayerStatePrepare, PlayerStatePrepare, pPlayer)
		CREATE_STATE(m_pPlayerStateBuffering, PlayerStateBuffering, pPlayer)
		CREATE_STATE(m_pPlayerStatePlaying, PlayerStatePlaying, pPlayer)
		CREATE_STATE(m_pPlayerStateStoping, PlayerStateStoping, pPlayer)
		CREATE_STATE(m_pPlayerStatePauseing, PlayerStatePauseing, pPlayer)
		CREATE_STATE(m_pPlayerStateSeeking, PlayerStateSeeking, pPlayer)
	m_pCurrentObject = m_pPlayerStatePrepare;
	return MTrue;
}

MVoid PlayerStateContext::SetState(State state)
{
	m_playerStateLock.Lock();
	if (state == State::Prepare)
	{
		m_pCurrentObject = m_pPlayerStatePrepare;
	}
	else if (state == State::Buffering)
	{
		m_pCurrentObject = m_pPlayerStateBuffering;
	}
	else if (state == State::Playing)
	{
		m_pCurrentObject = m_pPlayerStatePlaying;
	}
	else if (state == State::Stoping)
	{
		m_pCurrentObject = m_pPlayerStateStoping;
	}
	else if (state == State::Pauseing)
	{
		m_pCurrentObject = m_pPlayerStatePauseing;
	}
	else if (state == State::Seeking)
	{
		m_pCurrentObject = m_pPlayerStateSeeking;
	}
	m_playerStateLock.UnInit();
}

MVoid PlayerStateContext::Buffer()
{
	m_pCurrentObject->Buffer();
}

MBool PlayerStateContext::Play()
{

	return m_pCurrentObject->Play();
}

MVoid PlayerStateContext::Stop()
{
	//SetState(Stoping);
	m_pCurrentObject->Stop();
}

MVoid PlayerStateContext::Pause()
{
	//SetState(Pauseing);
	m_pCurrentObject->Pause();
}

MBool PlayerStateContext::Seek(MInt64 seekTime)
{
	//SetState(Seeking);
	//PlayerStateSeeking*  seekObj = (PlayerStateSeeking*)m_pCurrentObject;
	return m_pCurrentObject->Seek(seekTime);
}

