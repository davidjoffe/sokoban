//---------------------------------------------------------------------------
// djGraph.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djGraph.h"
#include "djUtils.h"
#include <cmath>

int djBlit(SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY)
{
	if (!pSrc || !pDest)
		return -1;
	float w = 0.0f;
	float h = 0.0f;
	SDL_GetTextureSize(pSrc, &w, &h);

	SDL_FRect dst{ static_cast<float>(iX), static_cast<float>(iY), w, h };
	return SDL_RenderTexture(pDest, pSrc, nullptr, &dst) ? 0 : -1;
}

int djBlit(SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY, int iSrcX, int iSrcY, int iW, int iH)
{
	if (!pSrc || !pDest)
		return -1;
	SDL_FRect src{ static_cast<float>(iSrcX), static_cast<float>(iSrcY), static_cast<float>(iW), static_cast<float>(iH) };
	SDL_FRect dst{ static_cast<float>(iX), static_cast<float>(iY), static_cast<float>(iW), static_cast<float>(iH) };
	return SDL_RenderTexture(pDest, pSrc, &src, &dst) ? 0 : -1;
}

int djDrawSprite(int iSprite, SDL_Renderer *pDest, SDL_Texture *pSrc, int iX, int iY, int iW, int iH)
{
	if (!pSrc || !pDest || iW <= 0 || iH <= 0)
		return -1;

	float texW = 0.0f;
	float texH = 0.0f;
	SDL_GetTextureSize(pSrc, &texW, &texH);

	int numCols = static_cast<int>(texW) / iW;
	if (numCols <= 0) return -1;

	SDL_FRect src{
		static_cast<float>((iSprite % numCols) * iW),
		static_cast<float>((iSprite / numCols) * iH),
		static_cast<float>(iW),
		static_cast<float>(iH)
	};
	SDL_FRect dst{
		static_cast<float>(iX),
		static_cast<float>(iY),
		static_cast<float>(iW),
		static_cast<float>(iH)
	};

	return SDL_RenderTexture(pDest, pSrc, &src, &dst) ? 0 : -1;
}

void djDrawRect(SDL_Renderer *pDest, int x, int y, int w, int h, SDL_Color colour)
{
	if (!pDest) return;
	SDL_SetRenderDrawColor(pDest, colour.r, colour.g, colour.b, colour.a);
	SDL_FRect r{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) };
	SDL_RenderRect(pDest, &r);
}

void djDrawFilledRect(SDL_Renderer *pDest, int x, int y, int w, int h, SDL_Color colour)
{
	if (!pDest) return;
	SDL_SetRenderDrawColor(pDest, colour.r, colour.g, colour.b, colour.a);
	SDL_FRect r{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h) };
	SDL_RenderFillRect(pDest, &r);
}

void djDrawGradientVert(SDL_Renderer *pDest, int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2, int nGranularity)
{
	if (!pDest || h <= 0) return;
	if (nGranularity <= 0) nGranularity = 1;
	int nNum = h / nGranularity;
	if (nNum <= 0) nNum = 1;

	float fSectionH = static_cast<float>(h) / static_cast<float>(nNum);
	for (int i = 0; i < nNum; ++i)
	{
		float fCoeff = static_cast<float>(i) / static_cast<float>(nNum);
		auto r = static_cast<Uint8>(r1 + static_cast<int>(static_cast<float>(r2 - r1) * fCoeff));
		auto g = static_cast<Uint8>(g1 + static_cast<int>(static_cast<float>(g2 - g1) * fCoeff));
		auto b = static_cast<Uint8>(b1 + static_cast<int>(static_cast<float>(b2 - b1) * fCoeff));

		float fY1 = std::floor(static_cast<float>(i) * fSectionH);
		float fY2 = std::ceil(static_cast<float>(i + 1) * fSectionH);

		SDL_SetRenderDrawColor(pDest, r, g, b, 255);
		SDL_FRect rect{
			static_cast<float>(x),
			static_cast<float>(y) + fY1,
			static_cast<float>(w),
			fY2 - fY1
		};
		SDL_RenderFillRect(pDest, &rect);
	}
}
