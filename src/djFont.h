//---------------------------------------------------------------------------
// djFont.h
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJFONT_H_
#define _DJFONT_H_

#include <SDL3/SDL.h>
#include <cstring>
#include "djUtils.h"

class CdjFont
{
public:
	CdjFont() = default;
	virtual ~CdjFont();

	bool LoadFont(SDL_Renderer *pRenderer, const char *szTextureFilename, int nW, int nH);
	void Destroy();

	int DrawText(SDL_Renderer *pDest, int iX, int iY, const char *szStr);

	int GetW() const noexcept { return m_nW; }
	int GetH() const noexcept { return m_nH; }
	int GetStringWidth(const char *szStr) const noexcept { return szStr ? static_cast<int>(std::strlen(szStr)) * m_nW : 0; }

protected:
	SDL_Texture *m_pFontTexture{nullptr};
	int m_nW{0};
	int m_nH{0};
};

#endif
