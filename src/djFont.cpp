//---------------------------------------------------------------------------
// djFont.cpp
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djFont.h"
#include "djGraph.h"
#include <cstring>

CdjFont::CdjFont() = default;

CdjFont::~CdjFont()
{
	Destroy();
}

void CdjFont::Destroy()
{
	if (m_pFontSurface) SDL_FreeSurface(m_pFontSurface);
	m_pFontSurface = nullptr;
	m_nW = 0;
	m_nH = 0;
}

bool CdjFont::LoadFont(const char *szTextureFilename, int nW, int nH)
{
	Destroy();
	SDL_Surface *pSurface = SDL_LoadBMP(szTextureFilename);
	if (pSurface)
	{
		SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(pSurface->format, 255, 0, 255));
		m_pFontSurface = SDL_DisplayFormat(pSurface);
		SDL_FreeSurface(pSurface);
	}

	m_nW = nW;
	m_nH = nH;
	return true;
}

int CdjFont::DrawText(SDL_Surface *pDest, int iX, int iY, const char *szStr)
{
	if (!pDest || !m_pFontSurface || m_nW <= 0 || m_nH <= 0 || !szStr)
		return -1;

	size_t len = std::strlen(szStr);
	for (size_t i = 0; i < len; ++i)
	{
		djDrawSprite(
			static_cast<int>(static_cast<unsigned char>(szStr[i])),
			pDest,
			m_pFontSurface,
			iX + static_cast<int>(i) * m_nW,
			iY,
			m_nW,
			m_nH);
	}
	return 0;
}
