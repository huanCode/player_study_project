#include "PlatformFactory.h"


#ifdef PLATFORM_WIN
#include "DisplayVideoWindow.h"
#include "AudioPlayWindow.h"
#endif

#ifdef PLATFORM_ANDROID

#endif


IAudioPlay* PlatformFactory::CreateAudioPlay()
{
	IAudioPlay* obj = MNull;

#ifdef PLATFORM_WIN
	obj = new AudioPlayWindow();
#endif

#ifdef PLATFORM_ANDROID

#endif



	return obj;
}



IDisplayVideo* PlatformFactory::CreateDisplayVideo()
{
	IDisplayVideo* obj = MNull;
#ifdef PLATFORM_WIN
	obj = new DisplayVideoWindow();
#endif

#ifdef PLATFORM_ANDROID

#endif

	return obj;
}