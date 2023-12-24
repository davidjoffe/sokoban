//---------------------------------------------------------------------------
// djGraph.h
//
// Copyright (C) 2001-2023 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
/*
\file   djGraph.h
\author David Joffe
\brief  Some simple SDL wrapper utility functions to simplify certain things
*/
#ifndef _DJGRAPH_H_
#define _DJGRAPH_H_

#include "SDL.h"
#include "djUtils.h"

//! Blit
extern int djBlit(SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY);
//! Blit
extern int djBlit(SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY, int iSrcX, int iSrcY, int iW, int iH);
//! Draw text
//extern int djDrawText(SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY, char *szStr);
//! Draw sprite
extern int djDrawSprite(int iSprite, SDL_Surface *pDest, SDL_Surface *pSrc, int iX, int iY, int iW, int iH);
//! Draw rectangle
extern void djDrawRect(SDL_Surface *pDest, int x, int y, int w, int h, UINT uColour);
//! Draw filled rectangle
extern void djDrawFilledRect(SDL_Surface *pDest, int x, int y, int w, int h, UINT uColour);
//! Draw rectangle filled with a vertical gradient from colour (r1,g1,b1) to (r2,g2,b2)
extern void djDrawGradientVert(SDL_Surface *pSurface, int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2, int nGranularity=2);

#endif
