//---------------------------------------------------------------------------
// djFont.h
//
// Copyright (C) 2002-2024 David Joffe
// https://grayskygames.com/
//---------------------------------------------------------------------------
#ifndef _DJFONT_H_
#define _DJFONT_H_

#include <SDL.h>
#include <string.h>
#include "djUtils.h"

//! Simple fixed-width raster font.
class CdjFont
{
public:
	//! Constructor
	CdjFont();
	//! Destructor
	virtual ~CdjFont();

	//! Initialize a font
	bool LoadFont(const char *szTextureFilename, int nW, int nH);
	//! Destroy font
	void Destroy();

	//! Draw text onto pDest at (iX, iY)
	int DrawText(SDL_Surface *pDest, int iX, int iY, const char *szStr);

	//! Get font character width [pixels]
	int GetW() const { return m_nW; }
	//! Get font character height [pixels]
	int GetH() const { return m_nH; }

	//! Return width in pixels that given string requires to be printed
	int GetStringWidth(const char *szStr) const { return strlen(szStr)*m_nW; }

protected:
	SDL_Surface *m_pFontSurface;	//!< Font texture
	int m_nW;						//!< Character width [pixels]
	int m_nH;						//!< Character height [pixels]
};

#endif
