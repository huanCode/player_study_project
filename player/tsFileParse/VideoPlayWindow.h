#pragma once
#ifndef _IVIDEOPLAYWINDOW_H
#define _IVIDEOPLAYWINDOW_H
#include "amcomdef.h"
#include "common.h"
extern "C"
{
#include "sdl/SDL.h"
};
class VideoPlayWindow
{
public:
	VideoPlayWindow();
	MBool Open();
	MVoid Close();

	MBool Display(MPChar pBuffer);
	MVoid SetVideoInfo(VideoInfo* info);

private:
	SDL_Window		*m_screen;
	SDL_Renderer	*m_sdlRenderer;
	SDL_Texture		*m_sdlTexture;
	SDL_Rect		m_sdlRect;
	MInt32		m_windowWidth;
	MInt32		m_windowHeight;



	VideoInfo	m_info;

};



#endif // !_IVIDEOPLAYWINDOW_H