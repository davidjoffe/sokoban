//---------------------------------------------------------------------------
// djControls.cpp
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------

#include "djControls.h"
#include "djDesktop.h"

//___________________________________________________________________________
//
// djTextEdit
//___________________________________________________________________________

djTextEdit::djTextEdit()
{
	m_szLabel = NULL;
	m_nPos = 0;
	m_uMaxLen = 40;
	m_fBlinkTimer = 0;
	m_bConfirmed = false;
}

void djTextEdit::SetText(const char *szLabel, const char *szDefaultValue)
{
	djDELV(m_szLabel);
	m_sText = "";
	m_szLabel = djStrDup(szLabel);
	m_sText = szDefaultValue;
	m_nPos = 0;
	m_nPos = m_sText.length();
	m_fBlinkTimer = 0;
	m_bConfirmed = false;
}

djTextEdit::~djTextEdit()
{
	djDELV(m_szLabel);
}

void djTextEdit::InsertChar(char c)
{
	if (m_sText.length()<m_uMaxLen)
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
			if (m_nPos>0) m_nPos--;
			return true;
		case SDLK_RIGHT:
			if (m_nPos<m_sText.length()) m_nPos++;
			return true;

		case SDLK_HOME:
			m_nPos = 0;
			return true;
		case SDLK_END:
			m_nPos = m_sText.length();
			return true;

		case SDLK_BACKSPACE:
			if (m_nPos>=1)
			{
				m_nPos--;
				m_sText.erase(m_sText.begin() + m_nPos);
			}
			return true;
		case SDLK_DELETE:
			if (m_nPos<(int)(m_sText.length()))
				m_sText.erase(m_sText.begin() + m_nPos);
			return true;

		case SDLK_RETURN:
		case SDLK_ESCAPE:
			m_bConfirmed = true;
			return true;

		case SDLK_SPACE		: InsertChar(' ')	; return true;
		case SDLK_EXCLAIM	: InsertChar('!')	; return true;
		case SDLK_QUOTEDBL	: InsertChar('"')	; return true;
		case SDLK_HASH		: InsertChar('#')	; return true;
		case SDLK_DOLLAR	: InsertChar('$')	; return true;
		case SDLK_AMPERSAND	: InsertChar('&')	; return true;
		case SDLK_QUOTE		: InsertChar('\'')	; return true;
		case SDLK_LEFTPAREN	: InsertChar('(')	; return true;
		case SDLK_RIGHTPAREN: InsertChar(')')	; return true;
		case SDLK_ASTERISK	: InsertChar('*')	; return true;
		case SDLK_PLUS		: InsertChar('+')	; return true;
		case SDLK_COMMA		: InsertChar(',')	; return true;
		case SDLK_MINUS		: InsertChar('-')	; return true;
		case SDLK_PERIOD	: InsertChar('.')	; return true;
		case SDLK_SLASH		: InsertChar('/')	; return true;

		case SDLK_COLON		:	InsertChar(':')	; return true;
		case SDLK_SEMICOLON	:	InsertChar(';')	; return true;
		case SDLK_LESS		:	InsertChar('<')	; return true;
		case SDLK_EQUALS	:	InsertChar('=')	; return true;
		case SDLK_GREATER	:	InsertChar('>')	; return true;
		case SDLK_QUESTION	:	InsertChar('?')	; return true;
		case SDLK_AT		:	InsertChar('@')	; return true;
		case SDLK_LEFTBRACKET:	InsertChar('[')	; return true;
		case SDLK_BACKSLASH	:	InsertChar('\\'); return true;
		case SDLK_RIGHTBRACKET:	InsertChar(']')	; return true;
		case SDLK_CARET		:	InsertChar('^')	; return true;
		case SDLK_UNDERSCORE:	InsertChar('_')	; return true;
		case SDLK_BACKQUOTE	:	InsertChar('`')	; return true;

		default:
			if (nKey>=SDLK_a && nKey<=SDLK_z)
			{
				if ((ModState & KMOD_CAPS)!=0)
					InsertChar((nKey - SDLK_a) + (((ModState & KMOD_SHIFT)!=0) ? 'a' : 'A'));
				else
					InsertChar((nKey - SDLK_a) + (((ModState & KMOD_SHIFT)!=0) ? 'A' : 'a'));
				return true;
			}
			else if (nKey>=SDLK_0 && nKey<=SDLK_9)
			{
				InsertChar((nKey - SDLK_0) + '0');
				return true;
			}
		}
	}
	return true; // Always claim to have handled keypresses - swallow all keys we don't handle, so system doesn't handle them.7
}

bool djTextEdit::Update(float dt)
{
	m_fBlinkTimer += dt;
	if (m_fBlinkTimer>=0.6f)
		m_fBlinkTimer -= 0.6f;
	return true;
}

void djTextEdit::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawGradientVert(pSurface, nX, nY, m_nW, m_nH, 0x52, 0xB5, 0x52, 0, 74, 74);
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format,255,255,255));

	CdjFont *pFont = g_pScreen->m_pFont8x13;
	int nFontH = pFont->GetH();
	int nCharW = pFont->GetW();
	if (m_szLabel)
		pFont->DrawText(pSurface, nX + (m_nW - strlen(m_szLabel)*8)/2, nY + 12, m_szLabel);

	bool bCenterText=true;
	if (bCenterText)
	{
		const int nMARGIN = 8;
		int nPosX = nX + (m_nW/2) - pFont->GetStringWidth(m_sText.c_str()) + (m_sText.length()-m_nPos)*pFont->GetW();
		int nDX = (nPosX<(nX+nMARGIN) ? (nX+nMARGIN)-nPosX : 0);
		if (nPosX+pFont->GetStringWidth(m_sText.c_str()) > nX+m_nW-nMARGIN)
			nDX = (nX+m_nW-nMARGIN) - (nPosX + pFont->GetStringWidth(m_sText.c_str()));
		int nPosY = nY + m_nH - (12 + nFontH);
		if (m_sText.c_str()) pFont->DrawText(pSurface, nPosX+nDX, nPosY, m_sText.c_str());
		if (m_fBlinkTimer<0.3f && !m_bConfirmed)
			djDrawRect(pSurface, nX + (m_nW/2) + nDX, nPosY - 1, 1, nFontH+1, SDL_MapRGB(pSurface->format,255,255,80));
	}
	else // Text at left, more 'normal'
	{
		int nPosX = nX + 12;
		int nPosY = nY + m_nH - (12 + nFontH);
		if (m_sText.c_str()) pFont->DrawText(pSurface, nPosX, nPosY, m_sText.c_str());
		if (m_fBlinkTimer<0.3f && !m_bConfirmed)
			djDrawRect(pSurface, nPosX+m_nPos*nCharW+1, nPosY - 1, 1, nFontH+1, SDL_MapRGB(pSurface->format,255,255,80));
	}
}

//___________________________________________________________________________
//
// CdjButton
//___________________________________________________________________________

CdjButton::CdjButton()
{
	m_szNormal = NULL;
	m_szHighlighted = NULL;
	m_szSelected = NULL;
	m_nState = 0;
	m_bPressed = false;
	m_bFocus = false;
}

CdjButton::~CdjButton()
{
	djDELV(m_szNormal);
	djDELV(m_szHighlighted);
	djDELV(m_szSelected);
}

void CdjButton::SetText(const char *szNormal, const char *szHighlighted, const char *szSelected)
{
	djDELV(m_szNormal);
	djDELV(m_szHighlighted);
	djDELV(m_szSelected);
	m_szNormal = djStrDup(szNormal);
	m_szHighlighted = djStrDup(szHighlighted);
	m_szSelected = djStrDup(szSelected);
}

bool CdjButton::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (m_bFocus && Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		int nKey = Event.key.keysym.sym;
		switch (nKey)
		{
		case SDLK_RETURN:
			m_bPressed = true;
			return true;
		}
	}
	
	return CdjItem::OnEvent(Event, ModState);
}

void CdjButton::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	CdjFont *pFont = g_pScreen->m_pFont8x13;

	// "Interior" X/Y/W/H -> The actual button area, minus main border
	int nIX = nX+1;
	int nIY = nY+1;
	int nIW = m_nW-2;
	int nIH = m_nH-2;

	UINT u, uL1, uL2, uD2, uD1;
	switch (m_nState)
	{
	case 1:
		u = SDL_MapRGB(pSurface->format, 110, 110, 110);
		uL1 = SDL_MapRGB(pSurface->format,180,180,180);
		uL2 = SDL_MapRGB(pSurface->format,150,150,150);
		uD2 = SDL_MapRGB(pSurface->format,70,70,70);
		uD1 = SDL_MapRGB(pSurface->format,40,40,40);
		break;
	case 2:
		u = SDL_MapRGB(pSurface->format, 100, 100, 100);
		uL1 = SDL_MapRGB(pSurface->format,150,150,150);
		uL2 = SDL_MapRGB(pSurface->format,130,130,130);
		uD2 = SDL_MapRGB(pSurface->format,60,60,60);
		uD1 = SDL_MapRGB(pSurface->format,30,30,30);
		nIX++;
		nIY++;
		break;
	default:
		u = SDL_MapRGB(pSurface->format, 130, 130, 130);
		uL1 = SDL_MapRGB(pSurface->format,180,180,180);
		uL2 = SDL_MapRGB(pSurface->format,150,150,150);
		uD2 = SDL_MapRGB(pSurface->format,70,70,70);
		uD1 = SDL_MapRGB(pSurface->format,40,40,40);
	}

	UINT uEdge = SDL_MapRGB(pSurface->format, 10, 10, 10);
	if (m_nState==2)
	{
		djDrawRect(pSurface, nX+1, nY+1, m_nW-2, 1, uEdge);
		djDrawRect(pSurface, nX+1, nY+1, 1, m_nH-2, uEdge);
	}

	djDrawFilledRect(pSurface, nIX, nIY, nIW, nIH, u);

	djDrawRect(pSurface, nIX  , nIY  ,   nIW,     1, uL1); // top
	djDrawRect(pSurface, nIX+1, nIY+1, nIW-2,     1, uL2); // top inner
	djDrawRect(pSurface, nIX  , nIY  ,     1,   nIH, uL1); // left
	djDrawRect(pSurface, nIX+1, nIY+1,     1, nIH-2, uL2); // left inner

	djDrawRect(pSurface, nIX      , nIY+nIH-1,   nIW,     1, uD1); // bottom
	djDrawRect(pSurface, nIX+1    , nIY+nIH-2, nIW-2,     1, uD2); // bottom inner
	djDrawRect(pSurface, nIX+nIW-1, nIY      ,     1,   nIH, uD1); // right
	djDrawRect(pSurface, nIX+nIW-2, nIY+1    ,     1, nIH-2, uD2); // right inner

	// Draw button text
	//int nFontW = pFont->GetW();
	const char *szText = NULL;
	switch (m_nState)
	{
	case 0: szText = m_szNormal; break;
	case 1: szText = m_szHighlighted; break;
	case 2: szText = m_szSelected; break;
	}
	if (szText)
	{
		int nFontH = pFont->GetH();
		pFont->DrawText(pSurface, nIX + (nIW - pFont->GetStringWidth(szText))/2, nIY + (nIH - nFontH)/2, szText);
		if (m_bFocus)
		{
			djDrawRect(pSurface,
				nIX + (nIW - pFont->GetStringWidth(szText))/2 - 8,
				nIY + (nIH - nFontH)/2 - 4,
				pFont->GetStringWidth(szText) + 16,
				pFont->GetH() + 8,
				SDL_MapRGB(pSurface->format,90,90,90));
		}
	}

	// Main outside border (drawn last because of state 2)
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, uEdge);
}

bool CdjButton::WasPressed()
{
	if (m_bPressed)
	{
		m_bPressed = false;
		return true;
	}
	return false;
}


//___________________________________________________________________________
//
// djList
//___________________________________________________________________________

const int SCROLL_HEIGHT = (16);

djList::djList()
{
	m_nOffset = 0;
	m_nSelected = 0;
	m_bSelectionChanged = false;
	m_nMX = -1;
	m_nMY = -1;
	m_nMax = -1;
	m_nScroll1X = -1;
	m_nScroll1Y = -1;
	m_nScroll1W = -1;
	m_nScroll1H = -1;
	m_nScroll2X = -1;
	m_nScroll2Y = -1;
	m_nScroll2W = -1;
	m_nScroll2H = -1;
}

djList::~djList()
{
}
 
bool djList::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		int nKey = Event.key.keysym.sym;
		switch (nKey)
		{
		case SDLK_UP:
			if (m_nSelected>0) m_nSelected--;
			m_bSelectionChanged = true;
			return true;
		case SDLK_DOWN:
			if (m_nSelected<m_aItems.size()-1) m_nSelected++;
			m_bSelectionChanged = true;
			return true;
		case SDLK_PAGEUP:
			if (m_aItems.size()>0)
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
			if (m_aItems.size()>0)
			{
				int nNewSel = djMIN(m_aItems.size()-1, m_nSelected + m_nMax);
				if (m_nSelected != nNewSel)
				{
					m_nSelected = nNewSel;
					m_bSelectionChanged = true;
				}
			}
			return true;
		case SDLK_HOME:
			if (m_aItems.size()>0 && m_nSelected!=0)
			{
				m_nSelected = 0;
				m_bSelectionChanged = true;
				m_nOffset = 0;
			}
			return true;
		case SDLK_END:
			if (m_aItems.size()>0 && m_nSelected!=m_aItems.size()-1)
			{
				m_nSelected = m_aItems.size()-1;
				m_bSelectionChanged = true;
			}
			return true;
		}
	}
	
	return CdjItem::OnEvent(Event, ModState);
}

void djList::OnMouseEnter(int nX, int nY)
{
}

bool djList::OnMouseMove(int nX, int nY, int nDX, int nDY)
{
	//djLog("%d %d %d %d", nX, nY, nDX, nDY);
	m_nMX = nX;
	m_nMY = nY;
	return false;
}

void djList::OnMouseLeave()
{
	m_nMX = -1;
	m_nMY = -1;
}

//! Return true if point (x,y) is (inclusively) inside the given rectangle
#define INBOUNDS(x,y,x1,y1,w,h) ( (x)>=(x1) && (x)<=((x1)+(w)) && (y)>=(y1) && (y)<=((y1)+(h)) )

bool djList::OnMouseButtonDown(int nButton)
{
	//djLog("[%d %d] %d %d %d %d", g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H);
	// Scroll up
	if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H))
	{
		if (m_nSelected>0) m_nSelected--;
		m_bSelectionChanged = true;
	}
	// Scroll down
	if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H))
	{
		if (m_nSelected<m_aItems.size()-1) m_nSelected++;
		m_bSelectionChanged = true;
	}
	if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll1X, m_nScroll1Y+m_nScroll1H, m_nScroll2W, m_nScroll2Y-(m_nScroll1Y+m_nScroll1H)))
	{
		CdjFont *pFont = g_pScreen->m_pFont8x13;
		int nSel = ((g_pScreen->m_nMouseY - (m_nScroll1Y+m_nScroll1H)) / pFont->GetH()) + m_nOffset;
		if (nSel<m_aItems.size()-1)
		{
			m_nSelected = nSel;
			m_bSelectionChanged = true;
		}
	}
	return true;
}

bool djList::OnMouseButtonUp(int nButton)
{
	return true;
}

bool djList::Update(float dt)
{
	return CdjItem::Update(dt);
}

void djList::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	// Background and border
	djDrawFilledRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 30, 30, 30));
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format,80,80,80));

	if (m_aItems.size()>0)
	{
		CdjFont *pFont = g_pScreen->m_pFont8x13;
		int nItemY = nY + 1 + SCROLL_HEIGHT;
		int nItemX = nX + 1;
		m_nMax = (m_nH-(2+SCROLL_HEIGHT+SCROLL_HEIGHT))/pFont->GetH();
		while (m_nSelected>=m_nOffset+m_nMax)
			m_nOffset++;
		while (m_nSelected<m_nOffset)
			m_nOffset--;

		// Mouse-over?
		int nMouseOver = -1;
		if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll1X, m_nScroll1Y+m_nScroll1H, m_nScroll2W, m_nScroll2Y-(m_nScroll1Y+m_nScroll1H)))
		{
			int nSel = ((g_pScreen->m_nMouseY - (m_nScroll1Y+m_nScroll1H)) / pFont->GetH()) + m_nOffset;
			if (nSel<m_aItems.size()-1)
				nMouseOver = nSel;
		}
		
		
		int i;
		for ( i=0; i<m_nMax; i++ )
		{
			int nItem = i+m_nOffset;
			if (nItem<m_aItems.size())
			{
				if (nItem==m_nSelected && nItem==nMouseOver)
				{
					// Lighter blue background for selected AND mouse-over item
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW-2, pFont->GetH(), SDL_MapRGB(pSurface->format,60,60,200));
				}
				else if (nItem==m_nSelected)
				{
					// Blue background for selected item
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW-2, pFont->GetH(), SDL_MapRGB(pSurface->format,30,30,180));
				}
				else if (nItem==nMouseOver) // Gray background for mouse-over item
				{
					djDrawFilledRect(pSurface, nItemX, nItemY, m_nW-2, pFont->GetH(), SDL_MapRGB(pSurface->format,50,50,50));
				}
				//pFont->DrawText(pSurface, nItemX, nItemY, m_aItems[nItem].asText[0].c_str());
				UINT j;
				for ( j=0; j<m_aItems[nItem].asText.size(); j++ )
				{
					int nColX = nItemX + (m_nW/3)*j;
					pFont->DrawText(pSurface, nColX, nItemY, m_aItems[nItem].asText[j].c_str());
				}
				nItemY += pFont->GetH();
			}
		}

		m_nScroll1X = nX+1;
		m_nScroll1Y = nY+1;
		m_nScroll1W = m_nW-2;
		m_nScroll1H = SCROLL_HEIGHT-1;
		m_nScroll2X = nX+1;
		m_nScroll2Y = nY+m_nH-SCROLL_HEIGHT;
		m_nScroll2W = m_nW-2;
		m_nScroll2H = SCROLL_HEIGHT-1;

		UINT uDisabled = SDL_MapRGB(pSurface->format, 60,  60,  60);
		UINT uEnabled = SDL_MapRGB(pSurface->format, 110, 110, 110);
		// Scroll bar: top
		if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H))
			uEnabled = SDL_MapRGB(pSurface->format, 160, 160, 160);
		else
			uEnabled = SDL_MapRGB(pSurface->format, 110, 110, 110);
		djDrawFilledRect(pSurface, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H, m_aItems.size()>1 ? uEnabled: uDisabled);
		djDrawRect(      pSurface, m_nScroll1X, m_nScroll1Y, m_nScroll1W, m_nScroll1H, SDL_MapRGB(pSurface->format,120,120,120));
		
		// Scroll bar: bottom
		if (INBOUNDS(g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H))
			uEnabled = SDL_MapRGB(pSurface->format, 160, 160, 160);
		else
			uEnabled = SDL_MapRGB(pSurface->format, 110, 110, 110);
		djDrawFilledRect(pSurface, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H, m_aItems.size()>1 ? uEnabled: uDisabled);
		djDrawRect(      pSurface, m_nScroll2X, m_nScroll2Y, m_nScroll2W, m_nScroll2H, SDL_MapRGB(pSurface->format,120,120,120));
	}
}

void djList::AddItem(const char *szItem)
{
	tsItem Item;
	Item.asText.push_back(szItem);
	m_aItems.push_back(Item);
}

void djList::SetItem(int nRow, int nColumn, const char *szItem)
{
	// Make sure we have enough rows
	while (m_aItems.size()-1<nRow)
		AddItem("");
	// Make sure we have enough columns in this row
	while (m_aItems[nRow].asText.size()-1<nColumn)
		m_aItems[nRow].asText.push_back("");
	m_aItems[nRow].asText[nColumn] = szItem;
}

bool djList::SelectionChanged()
{
	if (m_bSelectionChanged)
	{
		m_bSelectionChanged = false;
		return true;
	}
	return false;
}

void djList::SetSelection(int nSelected)
{
	m_nSelected = nSelected;
}

int djList::GetSelection()
{
	return m_nSelected;
}

//___________________________________________________________________________
//
// djMessageBox
//___________________________________________________________________________

djMessageBox::djMessageBox(const char *szText)
{
	m_sText = szText;
	m_pOK = NULL;
	m_pCancel = NULL;
	m_nRet = 0;
}

djMessageBox::~djMessageBox()
{
}

bool djMessageBox::Initialize()
{
	CdjItem::Initialize();

	int nButtonY = m_nH/2 + 10;
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
	m_pCancel->SetExtents(19+nButtonW+9, nButtonY, nButtonW, 30);
	AddChild(m_pCancel);

	return true;
}

bool djMessageBox::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		int nKey = Event.key.keysym.sym;
		switch (nKey)
		{
		case SDLK_ESCAPE:
			m_nRet = -1; // Cancel
			return true;
		}
	}
	
	return CdjItem::OnEvent(Event, ModState);
}

bool djMessageBox::Update(float dt)
{
	if (m_pCancel->WasPressed())
		m_nRet = -1;
	if (m_pOK->WasPressed())
		m_nRet = 1;

	return CdjItem::Update(dt);
}

void djMessageBox::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	// Background
	djDrawGradientVert(pSurface, nX, nY, m_nW, m_nH, 70, 50, 50, 180, 140, 140, 2);
	// Border
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format,250,250,250));

	CdjFont *pFont = g_pScreen->m_pFont8x13;
	int nTextX = nX + (m_nW - pFont->GetStringWidth(m_sText.c_str())) / 2;
	pFont->DrawText(pSurface, nTextX, nY+(m_nH/3)-(pFont->GetH()/2), m_sText.c_str());

	CdjItem::Draw(pSurface, nX, nY);
}
