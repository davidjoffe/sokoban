//---------------------------------------------------------------------------
// djDesktop.cpp
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------

#include "djDesktop.h"
#include "djSettings.h"

CdjDesktop *g_pScreen = NULL;

#define SETTING_FULLSCREENMODE	"DesktopFullScreenMode"
#define SETTING_SHOWDEBUGINFO	"DesktopShowDebugInfo"

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
	m_nHotspotX = 0;
	m_nHotspotY = 0;
	m_nMouseX = 0;
	m_nMouseY = 0;
	m_nMouseXPrev = 0;
	m_nMouseYPrev = 0;
	m_nMouseButtons = 0;
	m_pSurface = NULL;
	m_pCursor = NULL;
	m_pFont = NULL;
	m_pFont8x13 = NULL;
	m_pFont8x13B = NULL;
	m_bRunning = true;
	m_bShowCursor = true;
	m_fFPS = 0;
}

CdjDesktop::~CdjDesktop()
{
	Destroy();
}

bool CdjDesktop::Initialize()
{
	InitSurface();
	if (m_pSurface==NULL)
		return false;

	m_pFont = new CdjFont;
	m_pFont->LoadFont((GetDataPath() + "font.bmp").c_str(), 8, 8);
	m_pFont8x13 = new CdjFont;
	m_pFont8x13->LoadFont((GetDataPath() + "font8x13.bmp").c_str(), 8, 13);
	m_pFont8x13B = new CdjFont;
	m_pFont8x13B->LoadFont((GetDataPath() + "font8x13bold.bmp").c_str(), 8, 13);
	
	// Disable built-in mouse cursor
	SDL_ShowCursor(SDL_DISABLE);

	SetName("Desktop");
	
	//return LoadMouseCursor((GetDataPath() + "cursor.bmp").c_str());
	m_nHotspotX = 5;
	m_nHotspotY = 3;
	return LoadMouseCursorA((GetDataPath() + "cursor2.bmp").c_str(), (GetDataPath() + "cursor2_a.bmp").c_str());
}

bool CdjDesktop::LoadMouseCursor(const char *szFilename)
{
	SDL_Surface *pSurface = SDL_LoadBMP(szFilename);
	if (pSurface!=NULL)
	{
		SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(pSurface->format, 255, 0, 255));
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
		SDL_LockSurface(pAlpha);
		SDL_LockSurface(m_pCursor);
		int i, j;
		for ( i=0; i<m_pCursor->h; i++ )
		{
			for ( j=0; j<m_pCursor->w; j++ )
			{
				UINT *pRow = (UINT*)((char*)m_pCursor->pixels + i*m_pCursor->pitch);
				unsigned char ucAlpha = *( ((unsigned char*)pAlpha->pixels) + i*pAlpha->pitch + j );
				*(pRow + j) = (*(pRow+j) &= (0x00FFFFFF | (ucAlpha << 24)));
			}
		}
		SDL_UnlockSurface(m_pCursor);
		SDL_UnlockSurface(pAlpha);
		return true;
	}
	return false;
}

bool CdjDesktop::Destroy()
{
	if (m_pCursor!=NULL) SDL_FreeSurface(m_pCursor);
	m_pCursor = NULL;
	djDEL(m_pFont8x13);
	djDEL(m_pFont8x13B);
	djDEL(m_pFont);
	return true;
}

bool CdjDesktop::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_QUIT)
	{
		m_bRunning = false;
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
		case SDL_BUTTON_LEFT:	m_nMouseButtons |= 1; break;
		case SDL_BUTTON_MIDDLE:	m_nMouseButtons |= 2; break;
		case SDL_BUTTON_RIGHT:	m_nMouseButtons |= 4; break;
		}
		UpdateMouseButtonDownAll(this, Event.button.button);
	}
	if (Event.type == SDL_MOUSEBUTTONUP)
	{
		UpdateMouseButtonUpAll(this, Event.button.button);
		switch (Event.button.button)
		{
		case SDL_BUTTON_LEFT:	m_nMouseButtons &= ~1; break;
		case SDL_BUTTON_MIDDLE:	m_nMouseButtons &= ~2; break;
		case SDL_BUTTON_RIGHT:	m_nMouseButtons &= ~4; break;
		}
	}
	
	if (Event.type == SDL_KEYDOWN)
	{
		if (Event.key.type == SDL_KEYDOWN)
		{
			switch (Event.key.keysym.sym)
			{
				/*
			case SDLK_TAB:
				{
					// Ctrl+TAB - toggle debug info
					// FIXME: Ctrl doesn't have to be down??
					if ((ModState & KMOD_RCTRL) || (ModState & KMOD_LCTRL))
					{
						m_bShowDebugInfo = !m_bShowDebugInfo;
						g_Settings.SetSettingBool(SETTING_SHOWDEBUGINFO, m_bShowDebugInfo);
						SDL_ShowCursor(m_bShowDebugInfo ? SDL_ENABLE : SDL_DISABLE);
						return true;
					}
				}
				break;
				*/
			case SDLK_ESCAPE: m_bRunning = false; return true; // "Are you sure?" "yes".
			case SDLK_RETURN:
				{
					// Alt+enter - toggle fullscreen
					if ((ModState & KMOD_RALT) || (ModState & KMOD_LALT))
					{
						SetFullScreen(!m_bFullScreen);
						InitSurface();
						return true;
					}
				}
				break;
			default: // gcc, stop whining about unhandled enum values.
				break;
			}
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
	if (m_pSurface!=NULL) SDL_FreeSurface(m_pSurface);

	const int nWIDTH = 800;
	const int nHEIGHT = 600;
	const int nDEPTH = 32;

	if (m_bFullScreen)
	{
		m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
		// If failed, try some more conservative options ..
		if (m_pSurface==NULL)
		{
			m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
			if (m_pSurface==NULL)
			{
				SetFullScreen(false);
				m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE);
			}
		}
	}
	else
	{
		m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_HWSURFACE|SDL_DOUBLEBUF);
		if (m_pSurface==NULL)
			m_pSurface = SDL_SetVideoMode(nWIDTH, nHEIGHT, nDEPTH, SDL_SWSURFACE|SDL_DOUBLEBUF);
	}
	SetSize(nWIDTH, nHEIGHT);
	return 0;
}

bool CdjDesktop::Update(float dt)
{
	
	return true;
}

bool CdjDesktop::UpdateAfter(float dt)
{
	m_fFPS = 1.0f / (dt<0.001f ? 0.001f : dt);
		
	return true;
}

void CdjDesktop::DrawWidgetDebugInfo(CdjItem *pItem, int nScreenX, int nScreenY)
{
	// This
	//djLog("%s (%d,%d) (%d,%d %d,%d)", pItem->GetName()?pItem->GetName():"UNNAMED", nScreenX, nScreenY, pItem->m_nX, pItem->m_nY, pItem->m_nW, pItem->m_nH);
	if (pItem->m_nW>0 && pItem->m_nH>0)
	{
		djDrawRect(m_pSurface, nScreenX, nScreenY, pItem->m_nW, pItem->m_nH, SDL_MapRGB(m_pSurface->format, 255,255,255));
	}

	// Children
	for ( UINT i=0; i<pItem->GetNumChildren(); i++ )
	{
		CdjItem *pChild = pItem->GetChild(i);
		DrawWidgetDebugInfo(pChild, nScreenX + pChild->m_nX, nScreenY + pChild->m_nY);
	}
}

void CdjDesktop::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	// Clear background
	djDrawFilledRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format,0,0,0));
}

void CdjDesktop::DrawAll()
{
	CdjItem::DrawAll(m_pSurface, 0, 0);

	if (m_bShowDebugInfo)
	{
		// FPS indicator
		char sbuf[1024];
		sprintf(sbuf, "%.2f", m_fFPS);
		m_pFont->DrawText(m_pSurface, 2, 2, sbuf);
		DrawWidgetDebugInfo(this);
	}
}
