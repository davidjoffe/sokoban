//---------------------------------------------------------------------------
// djDesktop.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJDESKTOP_H_
#define _DJDESKTOP_H_

#include "djItem.h"
#include "djFont.h"
#include <string>
#include <atomic>

std::string GetDataPath();

class CdjDesktop : public CdjItem
{
public:
	CdjDesktop();
	~CdjDesktop() override;

	bool Initialize() override;
	bool Destroy() override;
	bool OnEvent(SDL_Event &Event, SDLMod ModState) override;

	bool Update(float dt) override;
	bool UpdateAfter(float dt) override;
	void Draw(SDL_Surface *pSurface, int nX, int nY) override;

	int InitSurface();
	bool LoadMouseCursor(const char *szFilename);
	bool LoadMouseCursorA(const char *szFilename, const char *szAlphaFilename);
	void ShowCursor() noexcept { m_bShowCursor = true; }
	void HideCursor() noexcept { m_bShowCursor = false; }

	std::atomic<bool> m_bRunning{true};
	bool m_bShowCursor{true};

	int m_nHotspotX{0};
	int m_nHotspotY{0};
	int m_nMouseX{0};
	int m_nMouseY{0};
	int m_nMouseXPrev{0};
	int m_nMouseYPrev{0};
	bool m_bFullScreen{false};
	int m_nMouseButtons{0};

	void SetMousePos(int nX, int nY) noexcept
	{
		m_nMouseXPrev = m_nMouseX;
		m_nMouseYPrev = m_nMouseY;
		m_nMouseX = nX;
		m_nMouseY = nY;
	}

	SDL_Surface *m_pCursor{nullptr};
	CdjFont *m_pFont{nullptr};
	CdjFont *m_pFont8x13{nullptr};
	CdjFont *m_pFont8x13B{nullptr};

	void DrawAll();
	SDL_Surface *GetSurface() const noexcept { return m_pSurface; }

protected:
	SDL_Surface *m_pSurface{nullptr};

	void SetFullScreen(bool bFullScreen);
	bool m_bShowDebugInfo{false};
	void DrawWidgetDebugInfo(CdjItem *pItem, int nScreenX = 0, int nScreenY = 0);
	float m_fFPS{0.0f};
};

extern CdjDesktop *g_pScreen;
extern std::atomic<bool> g_bMustRedraw;

#endif
