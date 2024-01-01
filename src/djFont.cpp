//---------------------------------------------------------------------------
// djFont.cpp
//
// Copyright (C) 2002-2024 David Joffe
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djFont.h"
#include "djGraph.h"

CdjFont::CdjFont()
{
	m_pFontSurface = NULL;
	m_nW = 0;
	m_nH = 0;
}

CdjFont::~CdjFont()
{
	Destroy();
}

void CdjFont::Destroy()
{
	if (m_pFontSurface) SDL_FreeSurface(m_pFontSurface);
	m_pFontSurface = NULL;
	m_nW = 0;
	m_nH = 0;
}

bool CdjFont::LoadFont(const char *szTextureFilename, int nW, int nH)
{
	Destroy();
	SDL_Surface *pSurface = SDL_LoadBMP(szTextureFilename);
	if (pSurface!=NULL)
	{
		SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(pSurface->format, 255, 0, 255));
		m_pFontSurface = SDL_DisplayFormat(pSurface);
		SDL_FreeSurface(pSurface);
	}

	m_nW = nW;
	m_nH = nH;
	return true;
}

int CdjFont::DrawText(SDL_Surface *pDest, int iX, int iY, const char *szStr)
{
	if (!pDest || !m_pFontSurface || m_nW<=0 || m_nH<=0)
		return -1;
	for ( UINT i=0; i<strlen(szStr); i++ )
	{
		djDrawSprite((int)(unsigned char)szStr[i], pDest, m_pFontSurface, iX+i*m_nW, iY, m_nW, m_nH);
	}
	return 0;
}
