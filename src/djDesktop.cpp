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
	InitSurface();
	if (!m_pSurface)
		return false;

	m_pFont = new CdjFont;
	m_pFont->LoadFont((GetDataPath() + "font.bmp").c_str(), 8, 8);
	m_pFont8x13 = new CdjFont;
	m_pFont8x13->LoadFont((GetDataPath() + "font8x13.bmp").c_str(), 8, 13);
	m_pFont8x13B = new CdjFont;
	m_pFont8x13B->LoadFont((GetDataPath() + "font8x13bold.bmp").c_str(), 8, 13);

	SDL_ShowCursor(SDL_DISABLE);
	SetName("Desktop");

	m_nHotspotX = 5;
	m_nHotspotY = 3;
	return LoadMouseCursorA((GetDataPath() + "cursor2.bmp").c_str(), (GetDataPath() + "cursor2_a.bmp").c_str());
}

bool CdjDesktop::LoadMouseCursor(const char *szFilename)
{
	SDL_Surface *pSurface = SDL_LoadBMP(szFilename);
	if (pSurface)
	{
		SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(pSurface->format, 255, 0, 255));
		m_pCursor = SDL_DisplayFormat(pSurface);
		SDL_FreeSurface(pSurface);
		return true;
	}
	return false;
}

bool CdjDesktop::LoadMouseCursorA(const char *szFilename, const char *szAlphaFilename)
{
	SDL_Surface *pSurface = SDL_LoadBMP(szFilename);
	if (pSurface)
	{
		m_pCursor = SDL_DisplayFormatAlpha(pSurface);
		SDL_FreeSurface(pSurface);
		SDL_Surface *pAlpha = SDL_LoadBMP(szAlphaFilename);
		if (!pAlpha)
			return false;

		SDL_LockSurface(pAlpha);
		SDL_LockSurface(m_pCursor);
		for (int i = 0; i < m_pCursor->h; ++i)
		{
			for (int j = 0; j < m_pCursor->w; ++j)
			{
				auto *pRow = reinterpret_cast<UINT*>(static_cast<char*>(m_pCursor->pixels) + i * m_pCursor->pitch);
				unsigned char ucAlpha = *(reinterpret_cast<unsigned char*>(pAlpha->pixels) + i * pAlpha->pitch + j);
				*(pRow + j) = (*(pRow + j) & 0x00FFFFFF) | (static_cast<UINT>(ucAlpha) << 24);
			}
		}
		SDL_UnlockSurface(m_pCursor);
		SDL_UnlockSurface(pAlpha);
		SDL_FreeSurface(pAlpha);
		return true;
	}
	return false;
}

bool CdjDesktop::Destroy()
{
	if (m_pCursor)
	{
		SDL_FreeSurface(m_pCursor);
		m_pCursor = nullptr;
	}
	delete m_pFont8x13;
	m_pFont8x13 = nullptr;

	delete m_pFont8x13B;
	m_pFont8x13B = nullptr;

	delete m_pFont;
	m_pFont = nullptr;

	return true;
}

bool CdjDesktop::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_QUIT)
	{
		m_bRunning.store(false, std::memory_order_relaxed);
		return true;
	}

	if (Event.type == SDL_MOUSEMOTION)
	{
		g_pScreen->SetMousePos(Event.motion.x, Event.motion.y);
		UpdateMouseAll(this);
	}

	if (Event.type == SDL_MOUSEBUTTONDOWN)
	{
		switch (Event.button.button)
		{
		case SDL_BUTTON_LEFT:   m_nMouseButtons |= 1; break;
		case SDL_BUTTON_MIDDLE: m_nMouseButtons |= 2; break;
		case SDL_BUTTON_RIGHT:  m_nMouseButtons |= 4; break;
		}
		UpdateMouseButtonDownAll(this, Event.button.button);
	}

	if (Event.type == SDL_MOUSEBUTTONUP)
	{
		UpdateMouseButtonUpAll(this, Event.button.button);
		switch (Event.button.button)
		{
		case SDL_BUTTON_LEFT:   m_nMouseButtons &= ~1; break;
		case SDL_BUTTON_MIDDLE: m_nMouseButtons &= ~2; break;
		case SDL_BUTTON_RIGHT:  m_nMouseButtons &= ~4; break;
		}
	}

	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		switch (Event.key.keysym.sym)
		{
		case SDLK_ESCAPE:
			m_bRunning.store(false, std::memory_order_relaxed);
			return true;
		case SDLK_RETURN:
			if ((ModState & KMOD_RALT) || (ModState & KMOD_LALT))
			{
				SetFullScreen(!m_bFullScreen);
				InitSurface();
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
}

int CdjDesktop::InitSurface()
{
	if (m_pSurface) SDL_FreeSurface(m_pSurface);

	constexpr int nWIDTH  = 800;
	constexpr int nHEIGHT = 600;
	constexpr int nDEPTH  = 32;

	if (m_bFullScreen)
	{
		m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
		if (!m_pSurface)
		{
			m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
			if (!m_pSurface)
			{
				SetFullScreen(false);
				m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE);
			}
		}
	}
	else
	{
		m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_HWSURFACE | SDL_DOUBLEBUF);
		if (!m_pSurface)
			m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE | SDL_DOUBLEBUF);
	}
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
		djDrawRect(m_pSurface, nScreenX, nScreenY, pItem->m_nW, pItem->m_nH, SDL_MapRGB(m_pSurface->format, 255, 255, 255));
	}

	for (size_t i = 0; i < pItem->GetNumChildren(); ++i)
	{
		auto *pChild = pItem->GetChild(i);
		DrawWidgetDebugInfo(pChild, nScreenX + pChild->m_nX, nScreenY + pChild->m_nY);
	}
}

void CdjDesktop::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawFilledRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 0, 0, 0));
}

void CdjDesktop::DrawAll()
{
	CdjItem::DrawAll(m_pSurface, 0, 0);

	if (m_bShowDebugInfo)
	{
		char sbuf[1024];
		snprintf(sbuf, sizeof(sbuf), "%.2f", m_fFPS);
		m_pFont->DrawText(m_pSurface, 2, 2, sbuf);
		DrawWidgetDebugInfo(this);
	}
}
