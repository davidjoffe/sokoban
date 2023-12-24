//---------------------------------------------------------------------------
// djDesktop.h
//
// Copyright (C) 2001-2023 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
#ifndef _DJDESKTOP_H_
#define _DJDESKTOP_H_

#include "djItem.h"
#include "djFont.h"
#include <string>

extern std::string GetDataPath();

/*!\brief Desktop widget
\author   David Joffe

This is a special widget that forms the root of the widget tree. There
is one instance of this widget for the application.
*/
class CdjDesktop : public CdjItem
{
public:
	CdjDesktop();
	virtual ~CdjDesktop();

	virtual bool Initialize();
	virtual bool Destroy();
	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState);

	virtual bool Update(float dt);
	virtual bool UpdateAfter(float dt);
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	int InitSurface();
	bool LoadMouseCursor(const char *szFilename);
	bool LoadMouseCursorA(const char *szFilename, const char *szAlphaFilename);
	void ShowCursor() { m_bShowCursor = true; }
	void HideCursor() { m_bShowCursor = false; }

	bool m_bRunning;

	bool m_bShowCursor;

	int m_nHotspotX; // Mouse cursor hotspot pixel offset X
	int m_nHotspotY; // Mouse cursor hotspot pixel offset Y
	int m_nMouseX;
	int m_nMouseY;
	int m_nMouseXPrev;
	int m_nMouseYPrev;
	bool m_bFullScreen;
	int m_nMouseButtons;

	void SetMousePos(int nX, int nY)
	{
		m_nMouseXPrev = m_nMouseX;
		m_nMouseYPrev = m_nMouseY;
		m_nMouseX = nX;
		m_nMouseY = nY;
	}

	SDL_Surface *m_pCursor;
	CdjFont *m_pFont;
	CdjFont *m_pFont8x13;
	CdjFont *m_pFont8x13B;

	void DrawAll();

	SDL_Surface *GetSurface() { return m_pSurface; }

protected:
	SDL_Surface *m_pSurface;

	// Set m_bFullScreen variable and update setting (g_Settings) to reflect the new value
	void SetFullScreen(bool bFullScreen);
	
	bool m_bShowDebugInfo;	//!< Show debug info
	//! Debug function
	void DrawWidgetDebugInfo(CdjItem *pItem, int nScreenX=0, int nScreenY=0);
	float m_fFPS;
};

//! The main desktop widget, root of all other widgets
extern CdjDesktop *g_pScreen;

//! Set to true by anything if the screen should be redrawn
extern bool g_bMustRedraw;

#endif
