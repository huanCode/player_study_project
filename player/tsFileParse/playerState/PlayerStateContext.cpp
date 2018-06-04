#include "PlayerStateContext.h"
#include "PlayerStatePrepare.h"
#include "PlayerStateBuffering.h"
#include "PlayerStatePlaying.h"
#include "PlayerStateStoping.h"
#include "PlayerStatePauseing.h"
#include "PlayerStateSeeking.h"

#define CREATE_STATE(object,class_name) 	if (!object) \
						{	\
							object = new class_name();	\
							if (!object)	\
								return MFalse; \
							object->SetContext(this); \
						}	\

PlayerStateContext::PlayerStateContext()
{
	m_pPlayerStatePrepare		= MNull;
	m_pPlayerStateBuffering		= MNull;
	m_pPlayerStatePlaying		= MNull;
	m_pPlayerStateStoping		= MNull;
	m_pPlayerStatePauseing		= MNull;
	m_pCurrentObject			= MNull;
}

MBool PlayerStateContext::Init()
{
	//if (!m_pPlayerStatePrepare)
	//{
	//	m_pPlayerStatePrepare = new PlayerStatePrepare();
	//	if (!m_pPlayerStatePrepare)
	//	{
	//		return MFalse;
	//	}
	//}
	CREATE_STATE(m_pPlayerStatePrepare, PlayerStatePrepare)
		CREATE_STATE(m_pPlayerStateBuffering, PlayerStateBuffering)
		CREATE_STATE(m_pPlayerStatePlaying, PlayerStatePlaying)
		CREATE_STATE(m_pPlayerStateStoping, PlayerStateStoping)
		CREATE_STATE(m_pPlayerStatePauseing, PlayerStatePauseing)
		CREATE_STATE(m_pPlayerStateSeeking, PlayerStateSeeking)
	m_pCurrentObject = m_pPlayerStatePrepare;
	return MTrue;
}

MVoid PlayerStateContext::SetState(State state)
{
	if (state == Prepare)
	{
		m_pCurrentObject = m_pPlayerStatePrepare;
	}
	else if (state == Buffering)
	{
		m_pCurrentObject = m_pPlayerStateBuffering;
	}
	else if (state == Playing)
	{
		m_pCurrentObject = m_pPlayerStatePlaying;
	}
	else if (state == Stoping)
	{
		m_pCurrentObject = m_pPlayerStateStoping;
	}
	else if (state == Pauseing)
	{
		m_pCurrentObject = m_pPlayerStatePauseing;
	}
	else if (state == Seeking)
	{
		m_pCurrentObject = m_pPlayerStatePrepare;
	}
	
}

MVoid PlayerStateContext::Play()
{
	m_pCurrentObject->Play();
}

MVoid PlayerStateContext::Stop()
{

}

MVoid PlayerStateContext::Pause()
{

}

MVoid PlayerStateContext::Seek()
{

}

