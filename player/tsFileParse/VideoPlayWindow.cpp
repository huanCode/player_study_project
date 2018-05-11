#include "stdafx.h"
#include "VideoPlayWindow.h"

VideoPlayWindow::VideoPlayWindow()
{
	m_screen = MNull;
	m_sdlRenderer = MNull;
	m_sdlTexture = MNull;
	m_windowWidth = 0;
	m_windowHeight = 0;

	m_videoWidth = 0;
	m_videoHeight = 0;
}

MBool VideoPlayWindow::Open()
{
	if (m_windowWidth <= 0 || m_windowHeight <= 0)
	{
		return MFalse;
	}


	if (SDL_Init(SDL_INIT_VIDEO)) {
		
		return MFalse;
	}
	m_screen = SDL_CreateWindow("Simplest Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_windowWidth, m_windowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

	SDL_Texture* sdlTexture = SDL_CreateTexture(m_sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, m_videoWidth, m_videoHeight);

	m_sdlRect.x = 0;
	m_sdlRect.y = 0;
	m_sdlRect.w = m_windowWidth;
	m_sdlRect.h = m_windowHeight;

	return MTrue;
}

MVoid VideoPlayWindow::Close()
{
	m_screen = MNull;
	m_sdlRenderer = MNull;
	m_sdlTexture = MNull;
	SDL_Quit();
}

MBool VideoPlayWindow::Display(MPChar pBuffer)
{
	SDL_UpdateTexture(m_sdlTexture, NULL, pBuffer, m_videoWidth);
	SDL_RenderClear(m_sdlRenderer);
	SDL_RenderCopy(m_sdlRenderer, m_sdlTexture, NULL, &m_sdlRect);
	SDL_RenderPresent(m_sdlRenderer);

	return MTrue;
}
