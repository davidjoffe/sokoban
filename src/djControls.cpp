//---------------------------------------------------------------------------
// djControls.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djControls.h"
#include "djDesktop.h"

//___________________________________________________________________________
//
// djTextEdit
//___________________________________________________________________________

void djTextEdit::SetText(std::string_view sLabel, std::string_view sDefaultValue)
{
	m_sLabel = sLabel;
	m_sText = sDefaultValue;
	m_nPos = m_sText.length();
	m_fBlinkTimer = 0.0f;
	m_bConfirmed = false;
}

void djTextEdit::InsertChar(char c)
{
	if (m_sText.length() < m_uMaxLen)
	{
		m_sText.insert(m_sText.begin() + m_nPos, c);
		m_nPos++;
	}
}

bool djTextEdit::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (!m_bConfirmed && Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		int nKey = Event.key.keysym.sym;
		switch (nKey)
		{
		case SDLK_LEFT:
			if (m_nPos > 0) m_nPos--;
			return true;
		case SDLK_RIGHT:
			if (m_nPos < m_sText.length()) m_nPos++;
			return true;
		case SDLK_HOME:
			m_nPos = 0;
			return true;
		case SDLK_END:
			m_nPos = m_sText.length();
			return true;
		case SDLK_BACKSPACE:
			if (m_nPos >= 1)
			{
				m_nPos--;
				m_sText.erase(m_sText.begin() + m_nPos);
			}
			return true;
		case SDLK_DELETE:
			if (m_nPos < m_sText.length())
				m_sText.erase(m_sText.begin() + m_nPos);
			return true;
		case SDLK_RETURN:
		case SDLK_ESCAPE:
			m_bConfirmed = true;
			return true;

		case SDLK_SPACE:       InsertChar(' '); return true;
		case SDLK_EXCLAIM:     InsertChar('!'); return true;
		case SDLK_QUOTEDBL:    InsertChar('"'); return true;
		case SDLK_HASH:        InsertChar('#'); return true;
		case SDLK_DOLLAR:      InsertChar('$'); return true;
		case SDLK_AMPERSAND:   InsertChar('&'); return true;
		case SDLK_QUOTE:       InsertChar('\''); return true;
		case SDLK_LEFTPAREN:   InsertChar('('); return true;
		case SDLK_RIGHTPAREN:  InsertChar(')'); return true;
		case SDLK_ASTERISK:    InsertChar('*'); return true;
		case SDLK_PLUS:        InsertChar('+'); return true;
		case SDLK_COMMA:       InsertChar(','); return true;
		case SDLK_MINUS:       InsertChar('-'); return true;
		case SDLK_PERIOD:      InsertChar('.'); return true;
		case SDLK_SLASH:       InsertChar('/'); return true;
		case SDLK_COLON:       InsertChar(':'); return true;
		case SDLK_SEMICOLON:   InsertChar(';'); return true;
		case SDLK_LESS:        InsertChar('<'); return true;
		case SDLK_EQUALS:      InsertChar('='); return true;
		case SDLK_GREATER:     InsertChar('>'); return true;
		case SDLK_QUESTION:    InsertChar('?'); return true;
		case SDLK_AT:          InsertChar('@'); return true;
		case SDLK_LEFTBRACKET: InsertChar('['); return true;
		case SDLK_BACKSLASH:   InsertChar('\\'); return true;
		case SDLK_RIGHTBRACKET:InsertChar(']'); return true;
		case SDLK_CARET:       InsertChar('^'); return true;
		case SDLK_UNDERSCORE:  InsertChar('_'); return true;
		case SDLK_BACKQUOTE:   InsertChar('`'); return true;

		default:
			if (nKey >= SDLK_a && nKey <= SDLK_z)
			{
				bool bShift = (ModState & KMOD_SHIFT) != 0;
				bool bCaps  = (ModState & KMOD_CAPS) != 0;
				char base = (bShift ^ bCaps) ? 'A' : 'a';
				InsertChar(static_cast<char>((nKey - SDLK_a) + base));
				return true;
			}
			else if (nKey >= SDLK_0 && nKey <= SDLK_9)
			{
				InsertChar(static_cast<char>((nKey - SDLK_0) + '0'));
				return true;
			}
		}
	}
	return true;
}

bool djTextEdit::Update(float dt)
{
	m_fBlinkTimer += dt;
	if (m_fBlinkTimer >= 0.6f)
		m_fBlinkTimer -= 0.6f;
	return true;
}

void djTextEdit::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawGradientVert(pSurface, nX, nY, m_nW, m_nH, 0x52, 0xB5, 0x52, 0, 74, 74);
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 255, 255, 255));

	CdjFont *pFont = g_pScreen->m_pFont8x13;
	int nFontH = pFont->GetH();

	if (!m_sLabel.empty())
		pFont->DrawText(pSurface, nX + (m_nW - static_cast<int>(m_sLabel.length()) * 8) / 2, nY + 12, m_sLabel.c_str());

	const int nMARGIN = 8;
	int nPosX = nX + (m_nW / 2) - pFont->GetStringWidth(m_sText.c_str()) + static_cast<int>(m_sText.length() - m_nPos) * pFont->GetW();
	int nDX = (nPosX < (nX + nMARGIN) ? (nX + nMARGIN) - nPosX : 0);
	if (nPosX + pFont->GetStringWidth(m_sText.c_str()) > nX + m_nW - nMARGIN)
		nDX = (nX + m_nW - nMARGIN) - (nPosX + pFont->GetStringWidth(m_sText.c_str()));
	int nPosY = nY + m_nH - (12 + nFontH);
	pFont->DrawText(pSurface, nPosX + nDX, nPosY, m_sText.c_str());
	if (m_fBlinkTimer < 0.3f && !m_bConfirmed)
		djDrawRect(pSurface, nX + (m_nW / 2) + nDX, nPosY - 1, 1, nFontH + 1, SDL_MapRGB(pSurface->format, 255, 255, 80));
}

//___________________________________________________________________________
//
// CdjButton
//___________________________________________________________________________

void CdjButton::SetText(std::string_view sNormal, std::string_view sHighlighted, std::string_view sSelected)
{
	m_sNormal = sNormal;
	m_sHighlighted = sHighlighted.empty() ? sNormal : sHighlighted;
	m_sSelected = sSelected.empty() ? sNormal : sSelected;
}

bool CdjButton::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (m_bFocus && Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		if (Event.key.keysym.sym == SDLK_RETURN)
		{
			m_bPressed = true;
			return true;
		}
	}
	return CdjItem::OnEvent(Event, ModState);
}

void CdjButton::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	CdjFont *pFont = g_pScreen->m_pFont8x13;

	int nIX = nX + 1;
	int nIY = nY + 1;
	int nIW = m_nW - 2;
	int nIH = m_nH - 2;

	UINT u, uL1, uL2, uD2, uD1;
	switch (m_nState)
	{
	case 1:
		u = SDL_MapRGB(pSurface->format, 110, 110, 110);
		uL1 = SDL_MapRGB(pSurface->format, 180, 180, 180);
		uL2 = SDL_MapRGB(pSurface->format, 150, 150, 150);
		uD2 = SDL_MapRGB(pSurface->format, 70, 70, 70);
		uD1 = SDL_MapRGB(pSurface->format, 40, 40, 40);
		break;
	case 2:
		u = SDL_MapRGB(pSurface->format, 100, 100, 100);
		uL1 = SDL_MapRGB(pSurface->format, 150, 150, 150);
		uL2 = SDL_MapRGB(pSurface->format, 130, 130, 130);
		uD2 = SDL_MapRGB(pSurface->format, 60, 60, 60);
		uD1 = SDL_MapRGB(pSurface->format, 30, 30, 30);
		nIX++;
		nIY++;
		break;
	default:
		u = SDL_MapRGB(pSurface->format, 130, 130, 130);
		uL1 = SDL_MapRGB(pSurface->format, 180, 180, 180);
		uL2 = SDL_MapRGB(pSurface->format, 150, 150, 150);
		uD2 = SDL_MapRGB(pSurface->format, 70, 70, 70);
		uD1 = SDL_MapRGB(pSurface->format, 40, 40, 40);
	}

	UINT uEdge = SDL_MapRGB(pSurface->format, 10, 10, 10);
	if (m_nState == 2)
	{
		djDrawRect(pSurface, nX + 1, nY + 1, m_nW - 2, 1, uEdge);
		djDrawRect(pSurface, nX + 1, nY + 1, 1, m_nH - 2, uEdge);
	}

	djDrawFilledRect(pSurface, nIX, nIY, nIW, nIH, u);

	djDrawRect(pSurface, nIX,     nIY,     nIW,     1, uL1);
	djDrawRect(pSurface, nIX + 1, nIY + 1, nIW - 2, 1, uL2);
	djDrawRect(pSurface, nIX,     nIY,     1,     nIH, uL1);
	djDrawRect(pSurface, nIX + 1, nIY + 1, 1, nIH - 2, uL2);

	djDrawRect(pSurface, nIX,         nIY + nIH - 1, nIW,     1, uD1);
	djDrawRect(pSurface, nIX + 1,     nIY + nIH - 2, nIW - 2, 1, uD2);
	djDrawRect(pSurface, nIX + nIW - 1, nIY,         1,     nIH, uD1);
	djDrawRect(pSurface, nIX + nIW - 2, nIY + 1,     1, nIH - 2, uD2);

	const std::string *psText = &m_sNormal;
	if (m_nState == 1) psText = &m_sHighlighted;
	else if (m_nState == 2) psText = &m_sSelected;

	if (!psText->empty())
	{
		int nFontH = pFont->GetH();
		pFont->DrawText(pSurface, nIX + (nIW - pFont->GetStringWidth(psText->c_str())) / 2, nIY + (nIH - nFontH) / 2, psText->c_str());
		if (m_bFocus)
		{
			djDrawRect(pSurface,
				nIX + (nIW - pFont->GetStringWidth(psText->c_str())) / 2 - 8,
				nIY + (nIH - nFontH) / 2 - 4,
				pFont->GetStringWidth(psText->c_str()) + 16,
				pFont->GetH() + 8,
				SDL_MapRGB(pSurface->format, 90, 90, 90));
		}
	}

	djDrawRect(pSurface, nX, nY, m_nW, m_nH, uEdge);
}

//___________________________________________________________________________
//
// djList
//___________________________________________________________________________

constexpr int SCROLL_HEIGHT = 16;

#define INBOUNDS(x,y,x1,y1,w,h) ( (x)>=(x1) && (x)<=((x1)+(w)) && (y)>=(y1) && (y)<=((y1)+(h)) )

bool djList::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		int nKey = Event.key.keysym.sym;
		switch (nKey)
		{
		case SDLK_UP:
			if (m_nSelected > 0)
			{
				m_nSelected--;
				m_bSelectionChanged = true;
			}
			return true;
		case SDLK_DOWN:
			if (m_nSelected + 1 < static_cast<int>(m_aItems.size()))
			{
				m_nSelected++;
				m_bSelectionChanged = true;
			}
			return true;
		case SDLK_PAGEUP:
			if (!m_aItems.empty())
			{
				int nNewSel = djMAX(0, m_nSelected - m_nMax);
				if (m_nSelected != nNewSel)
				{
					m_nSelected = nNewSel;
					m_bSelectionChanged = true;
				}
			}
			return true;
		case SDLK_PAGEDOWN:
			if (!m_aItems.empty())
			{
				int nNewSel = djMIN(static_cast<int>(m_aItems.size()) - 1, m_nSelected + m_nMax);
				if (m_nSelected != nNewSel)
				{
					m_nSelected = nNewSel;
					m_bSelectionChanged = true;
				}
			}
			return true;
		case SDLK_HOME:
			if (!m_aItems.empty() && m_nSelected != 0)
			{
				m_nSelected = 0;
				m_bSelectionChanged = true;
				m_nOffset = 0;
			}
			return true;
		case SDLK_END:
			if (!m_aItems.empty() && m_nSelected != static_cast<int>(m_aItems.size()) - 1)
			{
				m_nSelected = static_cast<int>(m_aItems.size()) - 1;
				m_bSelectionChanged = true;
			}
			return true;
		}
	}
	return CdjItem::OnEvent(Event, ModState);
}

void djList::OnMouseEnter(int, int) {}

bool djList::OnMouseMove(int nX, int nY, int, int)
{
	m_nMX = nX;
	m_nMY = nY;
	return false;
}

void djList::OnMouseLeave()
{
	m_nMX = -1;
	m_nMY = -1;
}

bool djList::OnMouseButtonDown(int)
{
	int mouseX = g_pScreen->m_nMouseX;
	int mouseY = g_pScreen->m_nMouseY;

	if (INBOUNDS(mouseX, mouseY, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H))
	{
		if (m_nSelected > 0)
		{
			m_nSelected--;
			m_bSelectionChanged = true;
		}
	}
	if (INBOUNDS(mouseX, mouseY, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H))
	{
		if (m_nSelected + 1 < static_cast<int>(m_aItems.size()))
		{
			m_nSelected++;
			m_bSelectionChanged = true;
		}
	}
	if (INBOUNDS(mouseX, mouseY, m_nScroll1X, m_nScroll1Y + m_nScroll1H, m_nScroll2W, m_nScroll2Y - (m_nScroll1Y + m_nScroll1H)))
	{
		CdjFont *pFont = g_pScreen->m_pFont8x13;
		int nSel = ((mouseY - (m_nScroll1Y + m_nScroll1H)) / pFont->GetH()) + m_nOffset;
		if (nSel >= 0 && nSel < static_cast<int>(m_aItems.size()))
		{
			m_nSelected = nSel;
			m_bSelectionChanged = true;
		}
	}
	return true;
}

bool djList::OnMouseButtonUp(int) { return true; }
bool djList::Update(float dt) { return CdjItem::Update(dt); }

void djList::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawFilledRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 30, 30, 30));
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 80, 80, 80));

	if (!m_aItems.empty())
	{
		CdjFont *pFont = g_pScreen->m_pFont8x13;
		int nItemY = nY + 1 + SCROLL_HEIGHT;
		int nItemX = nX + 1;
		m_nMax = (m_nH - (2 + SCROLL_HEIGHT * 2)) / pFont->GetH();

		while (m_nSelected >= m_nOffset + m_nMax) m_nOffset++;
		while (m_nSelected < m_nOffset) m_nOffset--;

		int nMouseOver = -1;
		int mouseX = g_pScreen->m_nMouseX;
		int mouseY = g_pScreen->m_nMouseY;

		if (INBOUNDS(mouseX, mouseY, m_nScroll1X, m_nScroll1Y + m_nScroll1H, m_nScroll2W, m_nScroll2Y - (m_nScroll1Y + m_nScroll1H)))
		{
			int nSel = ((mouseY - (m_nScroll1Y + m_nScroll1H)) / pFont->GetH()) + m_nOffset;
			if (nSel >= 0 && nSel < static_cast<int>(m_aItems.size()))
				nMouseOver = nSel;
		}

		for (int i = 0; i < m_nMax; ++i)
		{
			int nItem = i + m_nOffset;
			if (nItem < static_cast<int>(m_aItems.size()))
			{
				if (nItem == m_nSelected && nItem == nMouseOver)
				{
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW - 2, pFont->GetH(), SDL_MapRGB(pSurface->format, 60, 60, 200));
				}
				else if (nItem == m_nSelected)
				{
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW - 2, pFont->GetH(), SDL_MapRGB(pSurface->format, 30, 30, 180));
				}
				else if (nItem == nMouseOver)
				{
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW - 2, pFont->GetH(), SDL_MapRGB(pSurface->format, 50, 50, 50));
				}

				for (size_t j = 0; j < m_aItems[nItem].asText.size(); ++j)
				{
					int nColX = nItemX + (m_nW / 3) * static_cast<int>(j);
					pFont->DrawText(pSurface, nColX, nItemY, m_aItems[nItem].asText[j].c_str());
				}
				nItemY += pFont->GetH();
			}
		}

		m_nScroll1X = nX + 1;
		m_nScroll1Y = nY + 1;
		m_nScroll1W = m_nW - 2;
		m_nScroll1H = SCROLL_HEIGHT - 1;

		m_nScroll2X = nX + 1;
		m_nScroll2Y = nY + m_nH - SCROLL_HEIGHT;
		m_nScroll2W = m_nW - 2;
		m_nScroll2H = SCROLL_HEIGHT - 1;

		UINT uDisabled = SDL_MapRGB(pSurface->format, 60, 60, 60);
		UINT uEnabled  = SDL_MapRGB(pSurface->format, 110, 110, 110);

		if (INBOUNDS(mouseX, mouseY, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H))
			uEnabled = SDL_MapRGB(pSurface->format, 160, 160, 160);

		djDrawFilledRect(pSurface, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H, m_aItems.size() > 1 ? uEnabled : uDisabled);
		djDrawRect(pSurface, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H, SDL_MapRGB(pSurface->format, 120, 120, 120));

		uEnabled = SDL_MapRGB(pSurface->format, 110, 110, 110);
		if (INBOUNDS(mouseX, mouseY, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H))
			uEnabled = SDL_MapRGB(pSurface->format, 160, 160, 160);

		djDrawFilledRect(pSurface, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H, m_aItems.size() > 1 ? uEnabled : uDisabled);
		djDrawRect(pSurface, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H, SDL_MapRGB(pSurface->format, 120, 120, 120));
	}
}

void djList::AddItem(std::string_view sItem)
{
	m_aItems.push_back({ {std::string(sItem)} });
}

void djList::SetItem(size_t nRow, size_t nColumn, std::string_view sItem)
{
	while (m_aItems.size() <= nRow)
		AddItem("");
	while (m_aItems[nRow].asText.size() <= nColumn)
		m_aItems[nRow].asText.emplace_back("");
	m_aItems[nRow].asText[nColumn] = std::string(sItem);
}

bool djList::SelectionChanged() noexcept
{
	if (m_bSelectionChanged)
	{
		m_bSelectionChanged = false;
		return true;
	}
	return false;
}

//___________________________________________________________________________
//
// djMessageBox
//___________________________________________________________________________

djMessageBox::djMessageBox(std::string_view sText)
	: m_sText(sText)
{
}

bool djMessageBox::Initialize()
{
	CdjItem::Initialize();

	int nButtonY = m_nH / 2 + 10;
	int nButtonW = (m_nW - (8 + 19 + 19)) / 2;

	m_pOK = new CdjButton;
	m_pOK->SetName("OK");
	m_pOK->Initialize();
	m_pOK->SetText("OK", "OK", "OK");
	m_pOK->SetExtents(19, nButtonY, nButtonW, 30);
	m_pOK->SetFocus(true);
	AddChild(m_pOK);

	m_pCancel = new CdjButton;
	m_pCancel->SetName("Cancel");
	m_pCancel->Initialize();
	m_pCancel->SetText("Cancel", "Cancel", "Cancel");
	m_pCancel->SetExtents(19 + nButtonW + 9, nButtonY, nButtonW, 30);
	AddChild(m_pCancel);

	return true;
}

bool djMessageBox::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		if (Event.key.keysym.sym == SDLK_ESCAPE)
		{
			m_nRet = -1;
			return true;
		}
	}
	return CdjItem::OnEvent(Event, ModState);
}

bool djMessageBox::Update(float dt)
{
	if (m_pCancel && m_pCancel->WasPressed())
		m_nRet = -1;
	if (m_pOK && m_pOK->WasPressed())
		m_nRet = 1;

	return CdjItem::Update(dt);
}

void djMessageBox::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawGradientVert(pSurface, nX, nY, m_nW, m_nH, 70, 50, 50, 180, 140, 140, 2);
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 250, 250, 250));

	CdjFont *pFont = g_pScreen->m_pFont8x13;
	int nTextX = nX + (m_nW - pFont->GetStringWidth(m_sText.c_str())) / 2;
	pFont->DrawText(pSurface, nTextX, nY + (m_nH / 3) - (pFont->GetH() / 2), m_sText.c_str());

	CdjItem::Draw(pSurface, nX, nY);
}
