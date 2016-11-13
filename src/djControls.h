//---------------------------------------------------------------------------
// djControls.h
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
#ifndef _DJCONTROLS_H_
#define _DJCONTROLS_H_

#include "djItem.h"
#include <string>

//___________________________________________________________________________
//
// CdjButton
//___________________________________________________________________________

/*!\brief Button widget.
\author   David Joffe

Related: \ref CdjItem
*/
class CdjButton : public CdjItem
{
public:
	//! Constructor
	CdjButton();
	//! Destructor
	virtual ~CdjButton();

	//! Set button text
	void SetText(const char *szNormal, const char *szHighlighted=NULL, const char *szSelected=NULL);

	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState);
	virtual void OnMouseEnter(int nX, int nY) { m_nState = 1; }
	virtual void OnMouseLeave() { m_nState = 0; }
	virtual bool OnMouseButtonDown(int nButton) { m_nState = 2; return true; }
	virtual bool OnMouseButtonUp(int nButton) { if (m_nState==2) m_bPressed = true; m_nState = 1; return true; }
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	bool WasPressed();
	// If has focus, will generate 'press' event on 'Enter' pressed
	void SetFocus(bool bFocus) { m_bFocus = bFocus; }

protected:
	char *m_szNormal;
	char *m_szHighlighted;
	char *m_szSelected;
	int m_nState;
	bool m_bPressed;
	bool m_bFocus;
};

//___________________________________________________________________________
//
// djTextEdit
//___________________________________________________________________________

class djTextEdit : public CdjItem
{
public:
	djTextEdit();
	virtual ~djTextEdit();

	virtual void SetText(const char *szLabel, const char *szDefaultValue);

	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState);
	virtual bool Update(float dt);
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	bool IsConfirmed() const { return m_bConfirmed; }
	const char *GetText() const { return m_sText.c_str(); }

protected:
	void InsertChar(char c);

	char *m_szLabel;
	std::string m_sText;
	int m_nPos;
	UINT m_uMaxLen;
	float m_fBlinkTimer;
	bool m_bConfirmed;
};

//___________________________________________________________________________
//
// djList
//___________________________________________________________________________

class djList : public CdjItem
{
public:
	djList();
	virtual ~djList();

	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState);
	virtual void OnMouseEnter(int nX, int nY);
	virtual bool OnMouseMove(int nX, int nY, int nDX, int nDY);
	virtual void OnMouseLeave();
	virtual bool OnMouseButtonDown(int nButton);
	virtual bool OnMouseButtonUp(int nButton);
	virtual bool Update(float dt);
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	void AddItem(const char *szItem);
	void SetItem(int nRow, int nColumn, const char *szItem);

	bool SelectionChanged();
	void SetSelection(int nSelected);
	int GetSelection();

protected:
	struct tsItem
	{
		std::vector<std::string> asText;
	};
	std::vector<tsItem> m_aItems;

	int m_nMX;
	int m_nMY;

	int m_nMax;
	int m_nScroll1X;
	int m_nScroll1Y;
	int m_nScroll1W;
	int m_nScroll1H;
	int m_nScroll2X;
	int m_nScroll2Y;
	int m_nScroll2W;
	int m_nScroll2H;

	int m_nSelected;
	int m_nOffset;
	bool m_bSelectionChanged;
};

//___________________________________________________________________________
//
// djMessageBox
//___________________________________________________________________________

class djMessageBox : public CdjItem
{
public:
	djMessageBox(const char *szText);
	virtual ~djMessageBox();

	virtual bool Initialize();

	virtual bool Update(float dt);
	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState);
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	bool m_bClose;
	int m_nRet;

protected:
	std::string m_sText;
	CdjButton *m_pOK;
	CdjButton *m_pCancel;
};

#endif
