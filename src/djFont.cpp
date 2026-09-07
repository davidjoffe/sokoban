//---------------------------------------------------------------------------
// djFont.cpp
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djFont.h"
#include "djGraph.h"
#include <cstring>

CdjFont::~CdjFont()
{
	Destroy();
}

void CdjFont::Destroy()
{
	if (m_pFontTexture)
	{
		SDL_DestroyTexture(m_pFontTexture);
		m_pFontTexture = nullptr;
	}
	m_nW = 0;
	m_nH = 0;
}

bool CdjFont::LoadFont(SDL_Renderer *pRenderer, const char *szTextureFilename, int nW, int nH)
{
	Destroy();
	SDL_Surface *pSurface = SDL_LoadBMP(szTextureFilename);
	if (!pSurface)
		return false;

	SDL_SetSurfaceColorKey(pSurface, true, SDL_MapSurfaceRGB(pSurface, 255, 0, 255));
	m_pFontTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
	SDL_DestroySurface(pSurface);

	if (m_pFontTexture)
	{
		SDL_SetTextureScaleMode(m_pFontTexture, SDL_SCALEMODE_NEAREST);
	}

	m_nW = nW;
	m_nH = nH;
	return (m_pFontTexture != nullptr);
}

int CdjFont::DrawText(SDL_Renderer *pDest, int iX, int iY, const char *szStr)
{
	if (!pDest || !m_pFontTexture || m_nW <= 0 || m_nH <= 0 || !szStr)
		return -1;

	size_t len = std::strlen(szStr);
	for (size_t i = 0; i < len; ++i)
	{
		djDrawSprite(
			static_cast<int>(static_cast<unsigned char>(szStr[i])),
			pDest,
			m_pFontTexture,
			iX + static_cast<int>(i) * m_nW,
			iY,
			m_nW,
			m_nH);
	}
	return 0;
}
