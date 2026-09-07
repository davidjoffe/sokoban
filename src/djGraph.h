//---------------------------------------------------------------------------
// djGraph.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJGRAPH_H_
#define _DJGRAPH_H_

#include <SDL3/SDL.h>
#include "djUtils.h"

constexpr inline SDL_Color djRGB(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) noexcept
{
	return SDL_Color{r, g, b, a};
}

int djBlit(SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY);
int djBlit(SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY, int iSrcX, int iSrcY, int iW, int iH);
int djDrawSprite(int iSprite, SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY, int iW, int iH);

void djDrawRect(SDL_Renderer *pDest, int x, int y, int w, int h, SDL_Color colour);
void djDrawFilledRect(SDL_Renderer *pDest, int x, int y, int w, int h, SDL_Color colour);
void djDrawGradientVert(SDL_Renderer *pDest, int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2, int nGranularity = 2);

#endif
