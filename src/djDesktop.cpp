//---------------------------------------------------------------------------
// djDesktop.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djDesktop.h"
#include "djSettings.h"

CdjDesktop *g_pScreen = nullptr;

constexpr const char* SETTING_FULLSCREENMODE = "DesktopFullScreenMode";
constexpr const char* SETTING_SHOWDEBUGINFO  = "DesktopShowDebugInfo";

std::string GetDataPath()
{
	return std::string("data/");
}

CdjDesktop::CdjDesktop()
{
	g_Settings.SetDefaultSettingBool(SETTING_FULLSCREENMODE, false);
	m_bFullScreen = g_Settings.GetSettingBool(SETTING_FULLSCREENMODE);
	g_Settings.SetDefaultSettingBool(SETTING_SHOWDEBUGINFO, false);
	m_bShowDebugInfo = g_Settings.GetSettingBool(SETTING_SHOWDEBUGINFO);
}

CdjDesktop::~CdjDesktop()
{
	Destroy();
}

bool CdjDesktop::Initialize()
{
	if (InitWindowAndRenderer() != 0)
		return false;

	m_pFont = new CdjFont;
	m_pFont->LoadFont(m_pRenderer, (GetDataPath() + "font.bmp").c_str(), 8, 8);
	m_pFont8x13 = new CdjFont;
	m_pFont8x13->LoadFont(m_pRenderer, (GetDataPath() + "font8x13.bmp").c_str(), 8, 13);
	m_pFont8x13B = new CdjFont;
	m_pFont8x13B->LoadFont(m_pRenderer, (GetDataPath() + "font8x13bold.bmp").c_str(), 8, 13);

	SDL_HideCursor();
	SetName("Desktop");

	m_nHotspotX = 5;
	m_nHotspotY = 3;
	return LoadMouseCursorA((GetDataPath() + "cursor2.bmp").c_str(), (GetDataPath() + "cursor2_a.bmp").c_str());
}

bool CdjDesktop::LoadMouseCursorA(const char *szFilename, const char *szAlphaFilename)
{
	SDL_Surface *pSurface = SDL_LoadBMP(szFilename);
	SDL_Surface *pAlpha = SDL_LoadBMP(szAlphaFilename);
	if (!pSurface || !pAlpha)
	{
		if (pSurface) SDL_DestroySurface(pSurface);
		if (pAlpha) SDL_DestroySurface(pAlpha);
		return false;
	}

	SDL_Surface *pRGBA = SDL_ConvertSurface(pSurface, SDL_PIXELFORMAT_RGBA32);
	SDL_DestroySurface(pSurface);
	if (!pRGBA)
	{
		SDL_DestroySurface(pAlpha);
		return false;
	}

	for (int i = 0; i < pRGBA->h; ++i)
	{
		for (int j = 0; j < pRGBA->w; ++j)
		{
			auto *pPixel = reinterpret_cast<Uint32*>(static_cast<char*>(pRGBA->pixels) + i * pRGBA->pitch + j * 4);
			unsigned char ucAlpha = *(reinterpret_cast<unsigned char*>(pAlpha->pixels) + i * pAlpha->pitch + j);
			*pPixel = (*pPixel & 0x00FFFFFF) | (static_cast<Uint32>(ucAlpha) << 24);
		}
	}
	SDL_DestroySurface(pAlpha);

	m_pCursor = SDL_CreateTextureFromSurface(m_pRenderer, pRGBA);
	SDL_DestroySurface(pRGBA);

	if (m_pCursor)
	{
		SDL_SetTextureScaleMode(m_pCursor, SDL_SCALEMODE_NEAREST);
	}
	return (m_pCursor != nullptr);
}

bool CdjDesktop::Destroy()
{
	if (m_pCursor)
	{
		SDL_DestroyTexture(m_pCursor);
		m_pCursor = nullptr;
	}
	delete m_pFont8x13; m_pFont8x13 = nullptr;
	delete m_pFont8x13B; m_pFont8x13B = nullptr;
	delete m_pFont; m_pFont = nullptr;

	if (m_pRenderer)
	{
		SDL_DestroyRenderer(m_pRenderer);
		m_pRenderer = nullptr;
	}
	if (m_pWindow)
	{
		SDL_DestroyWindow(m_pWindow);
		m_pWindow = nullptr;
	}
	return true;
}

bool CdjDesktop::OnEvent(SDL_Event &Event, SDL_Keymod ModState)
{
	if (Event.type == SDL_EVENT_QUIT)
	{
		m_bRunning.store(false, std::memory_order_relaxed);
		return true;
	}

	if (Event.type == SDL_EVENT_MOUSE_MOTION)
	{
		g_pScreen->SetMousePos(static_cast<int>(Event.motion.x), static_cast<int>(Event.motion.y));
		UpdateMouseAll(this);
	}

	if (Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		switch (Event.button.button)
		{
		case SDL_BUTTON_LEFT:   m_nMouseButtons |= 1; break;
		case SDL_BUTTON_MIDDLE: m_nMouseButtons |= 2; break;
		case SDL_BUTTON_RIGHT:  m_nMouseButtons |= 4; break;
		}
		UpdateMouseButtonDownAll(this, Event.button.button);
	}

	if (Event.type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		UpdateMouseButtonUpAll(this, Event.button.button);
		switch (Event.button.button)
		{
		case SDL_BUTTON_LEFT:   m_nMouseButtons &= ~1; break;
		case SDL_BUTTON_MIDDLE: m_nMouseButtons &= ~2; break;
		case SDL_BUTTON_RIGHT:  m_nMouseButtons &= ~4; break;
		}
	}

	if (Event.type == SDL_EVENT_KEY_DOWN)
	{
		switch (Event.key.key)
		{
		case SDLK_ESCAPE:
			m_bRunning.store(false, std::memory_order_relaxed);
			return true;
		case SDLK_RETURN:
			if ((ModState & SDL_KMOD_ALT) != 0)
			{
				SetFullScreen(!m_bFullScreen);
				return true;
			}
			break;
		default:
			break;
		}
	}
	return false;
}

void CdjDesktop::SetFullScreen(bool bFullScreen)
{
	m_bFullScreen = bFullScreen;
	g_Settings.SetSettingBool(SETTING_FULLSCREENMODE, m_bFullScreen);
	if (m_pWindow)
	{
		SDL_SetWindowFullscreen(m_pWindow, m_bFullScreen);
	}
}

int CdjDesktop::InitWindowAndRenderer()
{
	constexpr int nWIDTH  = 800;
	constexpr int nHEIGHT = 600;

	Uint32 flags = SDL_WINDOW_RESIZABLE;
	if (m_bFullScreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	m_pWindow = SDL_CreateWindow("Still Yet Another Sokoban", nWIDTH, nHEIGHT, flags);
	if (!m_pWindow)
		return -1;

	m_pRenderer = SDL_CreateRenderer(m_pWindow, nullptr);
	if (!m_pRenderer)
		return -1;

	SDL_SetRenderLogicalPresentation(m_pRenderer, nWIDTH, nHEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SetSize(nWIDTH, nHEIGHT);
	return 0;
}

bool CdjDesktop::Update(float /*dt*/)
{
	return true;
}

bool CdjDesktop::UpdateAfter(float dt)
{
	m_fFPS = 1.0f / (dt < 0.001f ? 0.001f : dt);
	return true;
}

void CdjDesktop::DrawWidgetDebugInfo(CdjItem *pItem, int nScreenX, int nScreenY)
{
	if (pItem->m_nW > 0 && pItem->m_nH > 0)
	{
		djDrawRect(m_pRenderer, nScreenX, nScreenY, pItem->m_nW, pItem->m_nH, djRGB(255, 255, 255));
	}

	for (size_t i = 0; i < pItem->GetNumChildren(); ++i)
	{
		auto *pChild = pItem->GetChild(i);
		DrawWidgetDebugInfo(pChild, nScreenX + pChild->m_nX, nScreenY + pChild->m_nY);
	}
}

void CdjDesktop::Draw(SDL_Renderer *pRenderer, int nX, int nY)
{
	djDrawFilledRect(pRenderer, nX, nY, m_nW, m_nH, djRGB(0, 0, 0));
}

void CdjDesktop::DrawAll()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);

	CdjItem::DrawAll(m_pRenderer, 0, 0);

	if (m_bShowDebugInfo)
	{
		char sbuf[1024];
		snprintf(sbuf, sizeof(sbuf), "%.2f", m_fFPS);
		m_pFont->DrawText(m_pRenderer, 2, 2, sbuf);
		DrawWidgetDebugInfo(this);
	}
}
