//---------------------------------------------------------------------------
// djGraph.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djGraph.h"
#include "djUtils.h"
#include <cmath>

#define SDL_SETRECT(SDLRect, nX, nY, nW, nH) SDLRect.x = (nX); SDLRect.y = (nY); SDLRect.w = (nW); SDLRect.h = (nH);

int djBlit(SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY)
{
	if (!pSrc || !pDest)
		return -1;
	SDL_Rect rc;
	rc.x = static_cast<Sint16>(iX);
	rc.y = static_cast<Sint16>(iY);
	rc.w = static_cast<Uint16>(pSrc->w);
	rc.h = static_cast<Uint16>(pSrc->h);
	return SDL_BlitSurface(pSrc, nullptr, pDest, &rc);
}

int djBlit(SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY, int iSrcX, int iSrcY, int iW, int iH)
{
	if (!pSrc || !pDest)
		return -1;
	SDL_Rect rcSrc, rcDest;
	rcSrc.x  = static_cast<Sint16>(iSrcX);
	rcSrc.y  = static_cast<Sint16>(iSrcY);
	rcSrc.w  = static_cast<Uint16>(iW);
	rcSrc.h  = static_cast<Uint16>(iH);
	rcDest.x = static_cast<Sint16>(iX);
	rcDest.y = static_cast<Sint16>(iY);
	rcDest.w = static_cast<Uint16>(iW);
	rcDest.h = static_cast<Uint16>(iH);
	return SDL_BlitSurface(pSrc, &rcSrc, pDest, &rcDest);
}

int djDrawSprite(int iSprite, SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY, int iW, int iH)
{
	if (!pSrc || !pDest || iW <= 0 || iH <= 0)
		return -1;
	int numCols = pSrc->w / iW;
	if (numCols <= 0) return -1;

	SDL_Rect rcSrc, rcDest;
	rcSrc.x  = static_cast<Sint16>((iSprite % numCols) * iW);
	rcSrc.y  = static_cast<Sint16>((iSprite / numCols) * iH);
	rcSrc.w  = static_cast<Uint16>(iW);
	rcSrc.h  = static_cast<Uint16>(iH);
	rcDest.x = static_cast<Sint16>(iX);
	rcDest.y = static_cast<Sint16>(iY);
	rcDest.w = static_cast<Uint16>(iW);
	rcDest.h = static_cast<Uint16>(iH);

	if (rcSrc.y >= pSrc->h || rcDest.y >= pDest->h || rcDest.x >= pDest->w || rcSrc.x >= pSrc->w)
		return 0;
	return SDL_BlitSurface(pSrc, &rcSrc, pDest, &rcDest);
}

void djDrawRect(SDL_Surface *pDest, int x, int y, int w, int h, UINT uColour)
{
	if (!pDest) return;
	SDL_Rect Rect;
	SDL_SETRECT(Rect, x, y, w, 1);
	SDL_FillRect(pDest, &Rect, uColour);
	SDL_SETRECT(Rect, x, y, 1, h);
	SDL_FillRect(pDest, &Rect, uColour);
	SDL_SETRECT(Rect, x + w - 1, y, 1, h);
	SDL_FillRect(pDest, &Rect, uColour);
	SDL_SETRECT(Rect, x, y + h - 1, w, 1);
	SDL_FillRect(pDest, &Rect, uColour);
}

void djDrawFilledRect(SDL_Surface *pDest, int x, int y, int w, int h, UINT uColour)
{
	if (!pDest) return;
	SDL_Rect Rect;
	SDL_SETRECT(Rect, x, y, w, h);
	SDL_FillRect(pDest, &Rect, uColour);
}

void djDrawGradientVert(SDL_Surface *pSurface, int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2, int nGranularity)
{
	if (!pSurface || h <= 0) return;
	if (nGranularity <= 0) nGranularity = 1;
	int nNum = h / nGranularity;
	if (nNum <= 0) nNum = 1;

	float fSectionH = static_cast<float>(h) / static_cast<float>(nNum);
	for (int i = 0; i < nNum; ++i)
	{
		float fCoeff = static_cast<float>(i) / static_cast<float>(nNum);
		int r = r1 + static_cast<int>(static_cast<float>(r2 - r1) * fCoeff);
		int g = g1 + static_cast<int>(static_cast<float>(g2 - g1) * fCoeff);
		int b = b1 + static_cast<int>(static_cast<float>(b2 - b1) * fCoeff);
		float fY1 = std::floor(static_cast<float>(i) * fSectionH);
		float fY2 = std::ceil(static_cast<float>(i + 1) * fSectionH);
		djDrawFilledRect(pSurface, x, y + static_cast<int>(fY1), w, static_cast<int>(fY2 - fY1), SDL_MapRGB(pSurface->format, r, g, b));
	}
}
