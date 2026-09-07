//---------------------------------------------------------------------------
// djItem.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djItem.h"
#include "djDesktop.h"

CdjItem::CdjItem() = default;

CdjItem::~CdjItem()
{
	for (auto *child : m_apChildren)
	{
		delete child;
	}
	m_apChildren.clear();
}

void CdjItem::AddChild(CdjItem *pChild)
{
	if (pChild)
	{
		pChild->m_pParent = this;
		m_apChildren.push_back(pChild);
	}
}

bool CdjItem::HandleEvent(SDL_Event &Event, SDL_Keymod ModState)
{
	for (auto *child : m_apChildren)
	{
		if (child->HandleEvent(Event, ModState))
			return true;
	}
	return OnEvent(Event, ModState);
}

void CdjItem::DestroyAll()
{
	for (auto *child : m_apChildren)
	{
		child->DestroyAll();
	}
	Destroy();
}

bool CdjItem::UpdateAll(float dt)
{
	Update(dt);
	for (auto *child : m_apChildren)
	{
		child->UpdateAll(dt);
	}
	UpdateAfter(dt);
	return true;
}

void CdjItem::DrawAll(SDL_Renderer *pRenderer, int nX, int nY)
{
	Draw(pRenderer, nX + m_nX, nY + m_nY);
	for (auto *child : m_apChildren)
	{
		child->DrawAll(pRenderer, nX + m_nX, nY + m_nY);
	}
}

void CdjItem::RemoveChild(CdjItem *pItem)
{
	std::erase_if(m_apChildren, [pItem](CdjItem *child) {
		return child == pItem;
	});
}

bool UpdateMouseAll(CdjItem *pItem, int nX, int nY)
{
	bool bHandled = false;
	for (size_t i = 0; i < pItem->GetNumChildren(); ++i)
	{
		auto *child = pItem->GetChild(i);
		bHandled |= UpdateMouseAll(child, nX + child->m_nX, nY + child->m_nY);
	}

	int mouseX = g_pScreen->m_nMouseX;
	int mouseY = g_pScreen->m_nMouseY;

	if (mouseX >= nX && mouseY >= nY &&
		mouseX < nX + pItem->m_nW && mouseY < nY + pItem->m_nH)
	{
		if (!pItem->m_bMouseInside)
		{
			pItem->OnMouseEnter(mouseX - nX, mouseY - nY);
			pItem->m_bMouseInside = true;
		}
		if (!bHandled)
		{
			bHandled = true;
			pItem->OnMouseMove(
				mouseX - nX,
				mouseY - nY,
				mouseX - g_pScreen->m_nMouseXPrev,
				mouseY - g_pScreen->m_nMouseYPrev);
		}
	}
	else
	{
		if (pItem->m_bMouseInside)
		{
			pItem->OnMouseLeave();
			pItem->m_bMouseInside = false;
		}
	}
	return bHandled;
}

bool UpdateMouseButtonDownAll(CdjItem *pItem, int nButton, int nX, int nY)
{
	bool bHandled = false;
	for (size_t i = 0; i < pItem->GetNumChildren(); ++i)
	{
		auto *child = pItem->GetChild(i);
		bHandled |= UpdateMouseButtonDownAll(child, nButton, nX + child->m_nX, nY + child->m_nY);
	}
	if (!bHandled)
	{
		int mouseX = g_pScreen->m_nMouseX;
		int mouseY = g_pScreen->m_nMouseY;
		if (mouseX >= nX && mouseY >= nY &&
			mouseX <= nX + pItem->m_nW && mouseY <= nY + pItem->m_nH)
		{
			bHandled |= pItem->OnMouseButtonDown(nButton);
		}
	}
	return bHandled;
}

void UpdateMouseButtonUpAll(CdjItem *pItem, int nButton, int nX, int nY)
{
	for (size_t i = 0; i < pItem->GetNumChildren(); ++i)
	{
		auto *child = pItem->GetChild(i);
		UpdateMouseButtonUpAll(child, nButton, nX + child->m_nX, nY + child->m_nY);
	}
	int mouseX = g_pScreen->m_nMouseX;
	int mouseY = g_pScreen->m_nMouseY;
	if (mouseX >= nX && mouseY >= nY &&
		mouseX <= nX + pItem->m_nW && mouseY <= nY + pItem->m_nH)
	{
		pItem->OnMouseButtonUp(nButton);
	}
}

bool CdjItem::Update(float) { return true; }
bool CdjItem::UpdateAfter(float) { return true; }

#define TITLEBARHEIGHT (18)
#define BORDERWIDTH (1)
#define TWOBORDERWIDTH (BORDERWIDTH*2)

CdjWindow::CdjWindow() = default;

void CdjWindow::AddChild(CdjItem *pChild)
{
	CdjItem::AddChild(pChild);
	if (!pChild) return;

	int nX = pChild->m_nX;
	int nY = pChild->m_nY;
	int nW = pChild->m_nW;
	int nH = pChild->m_nH;
	SetExtents(
		nX - BORDERWIDTH,
		nY - (BORDERWIDTH + TITLEBARHEIGHT),
		nW + TWOBORDERWIDTH,
		nH + TWOBORDERWIDTH + TITLEBARHEIGHT);
	pChild->SetExtents(BORDERWIDTH, TITLEBARHEIGHT + BORDERWIDTH, nW, nH);
}

void CdjWindow::Draw(SDL_Renderer *pRenderer, int nX, int nY)
{
	djDrawRect(pRenderer, nX, nY, m_nW, m_nH, djRGB(200, 200, 200));
	djDrawFilledRect(pRenderer, nX + BORDERWIDTH, nY + BORDERWIDTH, m_nW - TWOBORDERWIDTH, TITLEBARHEIGHT, djRGB(0, 0, 128));

	if (!m_sName.empty() && g_pScreen && g_pScreen->m_pFont8x13B)
	{
		g_pScreen->m_pFont8x13B->DrawText(pRenderer, nX + 16, nY + 6, m_sName.c_str());
	}
}

void CdjWindow::OnMouseEnter(int, int) { m_bDragging = false; }

bool CdjWindow::OnMouseMove(int, int, int nDX, int nDY)
{
	if (m_bDragging)
	{
		m_nX += nDX;
		m_nY += nDY;
		return true;
	}
	return false;
}

void CdjWindow::OnMouseLeave() { m_bDragging = false; }

bool CdjWindow::OnMouseButtonDown(int)
{
	if (m_bMouseInside)
	{
		m_bDragging = true;
		return true;
	}
	return false;
}

bool CdjWindow::OnMouseButtonUp(int)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		return true;
	}
	return false;
}
