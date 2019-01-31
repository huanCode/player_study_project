#include "stdafx.h"
#include "DisplayVideoWindow.h"

DisplayVideoWindow::DisplayVideoWindow()
{
	m_screen = MNull;
	m_sdlRenderer = MNull;
	m_sdlTexture = MNull;


	
}


MBool DisplayVideoWindow::Open()
{
	if (m_info.width <= 0 || m_info.height <= 0)
	{
		return MFalse;
	}


	if (SDL_Init(SDL_INIT_VIDEO)) {
		
		return MFalse;
	}
	m_screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_info.width, m_info.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!m_screen) {

		return MFalse;
	}

	m_sdlRenderer = SDL_CreateRenderer(m_screen, -1, 0);
	if (!m_sdlRenderer)
	{
		return MFalse;
	}


	Uint32 pixformat = 0;

	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat = SDL_PIXELFORMAT_IYUV;

	m_sdlTexture = SDL_CreateTexture(m_sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, m_info.width, m_info.height);
	if (!m_sdlTexture)
	{
		return MFalse;
	}

	m_sdlRect.x = 0;
	m_sdlRect.y = 0;
	m_sdlRect.w = m_info.width;
	m_sdlRect.h = m_info.height;

	return MTrue;
}

MVoid DisplayVideoWindow::Close()
{
	m_screen = MNull;
	m_sdlRenderer = MNull;
	m_sdlTexture = MNull;
	SDL_Quit();
}

MVoid DisplayVideoWindow::Display(MPChar pBuffer)
{
	SDL_UpdateTexture(m_sdlTexture, NULL, pBuffer, m_info.width);
	SDL_RenderClear(m_sdlRenderer);
	SDL_RenderCopy(m_sdlRenderer, m_sdlTexture, NULL, &m_sdlRect);
	SDL_RenderPresent(m_sdlRenderer);


}
