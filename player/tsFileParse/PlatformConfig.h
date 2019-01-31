#ifndef _PLATFORM_CONFIG_H
#define _PLATFORM_CONFIG_H




#ifdef PLATFORM_WIN
#include <windows.h>
#define	MSleep(x) Sleep(x)

#endif

#ifdef PLATFORM_ANDROID
#include <unistd.h>

#define MSleep(x) usleep(x * 1000) //Œ¢√Î
#endif



#endif //_PLATFORM_CONFIG_H