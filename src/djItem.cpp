//---------------------------------------------------------------------------
// djItem.cpp
//
// Copyright (C) 2001-2024 David Joffe
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djItem.h"
#include "djDesktop.h" // FIXME

CdjItem::CdjItem()
{
	m_pParent = NULL;
	m_nX = 0;
	m_nY = 0;
	m_nW = 0;
	m_nH = 0;
	m_szName = NULL;
	m_bMouseInside = false;
}

CdjItem::~CdjItem()
{
	djDELV(m_szName);
	while (m_apChildren.size()>0)
	{
		djDEL(m_apChildren[0]);
		m_apChildren.erase(m_apChildren.begin());
	}
}

void CdjItem::SetName(const char *szName)
{
	djDELV(m_szName);
	m_szName = djStrDup(szName);
}

bool CdjItem::HandleEvent(SDL_Event &Event, SDLMod ModState)
{
	// First check if children handle this
	for ( UINT i=0; i<m_apChildren.size(); i++ )
	{
		if (m_apChildren[i]->HandleEvent(Event, ModState))
			return true;
	}
	return OnEvent(Event, ModState);
}

void CdjItem::DestroyAll()
{
	for ( UINT i=0; i<m_apChildren.size(); i++ )
	{
		m_apChildren[i]->DestroyAll();
	}
	Destroy();
}

bool CdjItem::UpdateAll(float dt)
{
	Update(dt);
	for ( UINT i=0; i<m_apChildren.size(); i++ )
	{
		m_apChildren[i]->UpdateAll(dt);
	}
	UpdateAfter(dt);
	return true;
}

void CdjItem::DrawAll(SDL_Surface *pSurface, int nX, int nY)
{
	// This
	Draw(pSurface, nX+m_nX, nY+m_nY);
	// Children
	for ( UINT i=0; i<GetNumChildren(); i++ )
	{
		GetChild(i)->DrawAll(pSurface, nX+m_nX, nY+m_nY);
	}
}

void CdjItem::RemoveChild(CdjItem *pItem)
{
	for ( UINT i=0; i<GetNumChildren(); i++ )
	{
		if (GetChild(i) == pItem)
		{
			m_apChildren.erase(m_apChildren.begin() + i);
			return;
		}
	}
}

bool UpdateMouseAll(CdjItem *pItem, int nX, int nY)
{
	bool bHandled = false;
	// Children (depth-first recursion: give lower-down (higher-up on screen) windows a chance to handle this first)
	for ( UINT i=0; i<pItem->GetNumChildren(); i++ )
	{
		bHandled |= UpdateMouseAll(pItem->GetChild(i), nX+pItem->GetChild(i)->m_nX, nY+pItem->GetChild(i)->m_nY);
	}
	
	// This
	if (g_pScreen->m_nMouseX >= nX/*+pItem->m_nX*/             && g_pScreen->m_nMouseY >= nY/*+pItem->m_nY*/ &&
		g_pScreen->m_nMouseX <  nX/*+pItem->m_nX*/+pItem->m_nW && g_pScreen->m_nMouseY <  nY/*+pItem->m_nY*/+pItem->m_nH)
	{
		if (!pItem->m_bMouseInside)
		{
			// OnMouseEnter
			djLog("%s:OnMouseEnter (%d %d)", pItem->GetName(), nX, nY);
			pItem->OnMouseEnter(g_pScreen->m_nMouseX - nX, g_pScreen->m_nMouseY - nY);
			pItem->m_bMouseInside = true;
		}
		// OnMouseMove
		if (!bHandled)
		{
			bHandled = true;
			pItem->OnMouseMove(
				g_pScreen->m_nMouseX - nX,
				g_pScreen->m_nMouseY - nY,
				g_pScreen->m_nMouseX - g_pScreen->m_nMouseXPrev,
				g_pScreen->m_nMouseY - g_pScreen->m_nMouseYPrev);
		}
	}
	else
	{
		if (pItem->m_bMouseInside)
		{
			// OnMouseLeave
			djLog("%s:OnMouseLeave", pItem->GetName());
			pItem->OnMouseLeave();
			pItem->m_bMouseInside = false;
		}
	}
	return bHandled;
}

bool UpdateMouseButtonDownAll(CdjItem *pItem, int nButton, int nX, int nY)
{
	bool bHandled = false;
	// Children
	for ( UINT i=0; i<pItem->GetNumChildren(); i++ )
	{
		bHandled |= UpdateMouseButtonDownAll(pItem->GetChild(i), nButton, nX+pItem->GetChild(i)->m_nX, nY+pItem->GetChild(i)->m_nY);
	}
	// This
	if (!bHandled)
	{
		if (g_pScreen->m_nMouseX>=nX/*+pItem->m_nX*/ && g_pScreen->m_nMouseY>=nY/*+pItem->m_nY*/ &&
			g_pScreen->m_nMouseX<=nX/*+pItem->m_nX*/+pItem->m_nW && g_pScreen->m_nMouseY<=nY/*+pItem->m_nY*/+pItem->m_nH)
		{
			bHandled |= pItem->OnMouseButtonDown(nButton);
		}
	}
	return bHandled;
}

void UpdateMouseButtonUpAll(CdjItem *pItem, int nButton, int nX, int nY)
{
	// Children
	for ( UINT i=0; i<pItem->GetNumChildren(); i++ )
	{
		UpdateMouseButtonUpAll(pItem->GetChild(i), nButton, nX+pItem->GetChild(i)->m_nX, nY+pItem->GetChild(i)->m_nY);
	}
	// This
	if (g_pScreen->m_nMouseX>=nX/*+pItem->m_nX*/ && g_pScreen->m_nMouseY>=nY/*+pItem->m_nY*/ &&
		g_pScreen->m_nMouseX<=nX/*+pItem->m_nX*/+pItem->m_nW && g_pScreen->m_nMouseY<=nY/*+pItem->m_nY*/+pItem->m_nH)
	{
		if (pItem->OnMouseButtonUp(nButton))
			return;
	}
}

bool CdjItem::Update(float dt)
{
	return true;
}

bool CdjItem::UpdateAfter(float dt)
{
	return true;
}


#define TITLEBARHEIGHT (18)
#define BORDERWIDTH (1)
#define TWOBORDERWIDTH (BORDERWIDTH*2)

CdjWindow::CdjWindow()
{
	m_bDragging = false;
}

CdjWindow::~CdjWindow()
{
}

void CdjWindow::AddChild(CdjItem *pChild)
{
	CdjItem::AddChild(pChild);

	// Resize self to surround child extents
	int nX = pChild->m_nX;
	int nY = pChild->m_nY;
	int nW = pChild->m_nW;
	int nH = pChild->m_nH;
	SetExtents(
		nX - BORDERWIDTH,
		nY - (BORDERWIDTH+TITLEBARHEIGHT),
		nW + TWOBORDERWIDTH,
		nH + TWOBORDERWIDTH+TITLEBARHEIGHT);
	pChild->SetExtents(BORDERWIDTH, TITLEBARHEIGHT+BORDERWIDTH, nW, nH);
}

void CdjWindow::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	// Border
	//if (BORDERWIDTH>1)
	//	djDrawRect(pSurface, nX+BORDERWIDTH, nY+BORDERWIDTH, m_nW-TWOBORDERWIDTH, m_nH-TWOBORDERWIDTH, SDL_MapRGB(pSurface->format, 192,192,192));
	//djDrawRect(pSurface, nX, nY, 1, m_nH, SDL_MapRGB(pSurface->format, 255,255,255));
	//djDrawRect(pSurface, nX, nY, m_nW, 1, SDL_MapRGB(pSurface->format, 255,255,255));
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 200,200,200));

	// Title bar
	djDrawFilledRect(pSurface, nX+BORDERWIDTH, nY+BORDERWIDTH, m_nW-TWOBORDERWIDTH, TITLEBARHEIGHT, SDL_MapRGB(pSurface->format, 0,0,128));

	if (m_szName)
	{
		g_pScreen->m_pFont8x13B->DrawText(pSurface,
			nX + 16,
			nY + 6,
			m_szName);
	}
}

void CdjWindow::OnMouseEnter(int nX, int nY)
{
	m_bDragging = false;
}

bool CdjWindow::OnMouseMove(int nX, int nY, int nDX, int nDY)
{
	if (m_bDragging)
	{
		m_nX += nDX;
		m_nY += nDY;
		return true;
	}
	return false;
}

void CdjWindow::OnMouseLeave()
{
	m_bDragging = false;
}

bool CdjWindow::OnMouseButtonDown(int nButton)
{
	if (m_bMouseInside)
	{
		m_bDragging = true;
		return true;
	}
	return false;
}

bool CdjWindow::OnMouseButtonUp(int nButton)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		return true;
	}
	return false;
}
