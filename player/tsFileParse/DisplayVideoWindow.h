#pragma once
#ifndef _IDISPLAYVIDEOWINDOW_H
#define _IDISPLAYVIDEOWINDOW_H
#include "amcomdef.h"
#include "IDisplayVideo.h"

extern "C"
{
#include "sdl/SDL.h"
};
class DisplayVideoWindow:public IDisplayVideo
{
public:
	DisplayVideoWindow();
	MBool Open();
	MVoid Close();

	MVoid Display(MPChar pBuffer);


private:
	SDL_Window		*m_screen;
	SDL_Renderer	*m_sdlRenderer;
	SDL_Texture		*m_sdlTexture;
	SDL_Rect		m_sdlRect;

};



#endif // !_IDISPLAYVIDEOWINDOW_H