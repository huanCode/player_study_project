#ifndef _PLATFORM_FACTORY_H_
#define _PLATFORM_FACTORY_H_
#include "IAudioPlay.h"
#include "IDisplayVideo.h"
class PlatformFactory
{
public:
	static IAudioPlay* CreateAudioPlay();
	static IDisplayVideo* CreateDisplayVideo();
};


#endif //_PLATFORM_FACTORY_H_