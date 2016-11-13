//---------------------------------------------------------------------------
// djItem.h
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
#ifndef _DJITEM_H_
#define _DJITEM_H_

#include <SDL.h>
#include "djGraph.h"

#include "djUtils.h"

#include <vector>

/*!\brief Base widget class.
\author   David Joffe

An item is a single user interface entity, such as a button or static text item.

Each item contains a list of child items, thus each item is a node in a tree of
items.

An item can be complex; i.e. an item can be basically an entire game.

Items receive user interface events, handled by overriding virtual functions.

Each app (e.g. CSokoban) is eventually just a special derived CdjItem.
*/
class CdjItem
{
public:
	//! Constructor
	CdjItem();
	//! Destructor
	virtual ~CdjItem();


	//
	//
	// Overridables
	//
	//


	virtual bool Initialize() { return true; }
	virtual bool Destroy() { return true; }
	virtual bool OnEvent(SDL_Event &Event, SDLMod ModState) { return false; }
	//! Handle SDL event. This first checks if any child widgets handle the
	//! event. If not, returns the value returned by OnEvent(). You should
	//! rather override OnEvent() if you want to handle this.
	virtual bool HandleEvent(SDL_Event &Event, SDLMod ModState);

	virtual bool Update(float dt);
	virtual bool UpdateAfter(float dt);

	virtual void Draw(SDL_Surface *pSurface, int nX, int nY) {}
	virtual void OnMouseEnter(int nX, int nY) {}
	virtual bool OnMouseMove(int nX, int nY, int nDX, int nDY) { return false; }
	virtual void OnMouseLeave() {}
	virtual bool OnMouseButtonDown(int nButton) { return false; }
	virtual bool OnMouseButtonUp(int nButton) { return false; }


	bool UpdateAll(float dt);
	void DestroyAll();
	void DrawAll(SDL_Surface *pSurface, int nX, int nY);

	//! Add a child widget
	virtual void AddChild(CdjItem *pChild) { pChild->m_pParent = this; m_apChildren.push_back(pChild); }
	//! Get number of child widgets
	UINT GetNumChildren() const { return m_apChildren.size(); }
	//! Get child at a specific index
	CdjItem *GetChild(UINT uIndex) { return m_apChildren[uIndex]; }
	void RemoveChild(CdjItem *pItem);
	
	//! Pointer back to parent widget
	CdjItem *GetParent() { return m_pParent; }

	//! Set position (x, y in pixels, 0-based)
	void SetPos(int x, int y) { m_nX = x; m_nY = y; }
	//! Set size (width, height in pixels)
	void SetSize(int w, int h) { m_nW = w; m_nH = h; }
	//! Set position and size
	void SetExtents(int x, int y, int w, int h) { m_nX = x; m_nY = y; m_nW = w; m_nH = h; }

	int m_nX; //!< X position [pixels] 0-based
	int m_nY; //!< Y position [pixels] 0-based
	int m_nW; //!< Width [pixels]
	int m_nH; //!< Height [pixels]
	const char *GetName() const { return m_szName; }
	void SetName(const char *szName);
	bool m_bMouseInside;
protected:
	CdjItem *m_pParent;					//!< Pointer back to parent widget
	std::vector<CdjItem *> m_apChildren;//!< Array of child widgets
	char *m_szName;
};

extern bool UpdateMouseAll(CdjItem *pItem, int nX=0, int nY=0);
extern bool UpdateMouseButtonDownAll(CdjItem *pItem, int nButton, int nX=0, int nY=0);
extern void UpdateMouseButtonUpAll(CdjItem *pItem, int nButton, int nX=0, int nY=0);

class CdjWindow : public CdjItem
{
public:
	CdjWindow();
	virtual ~CdjWindow();

	virtual void AddChild(CdjItem *pChild);
	virtual void Draw(SDL_Surface *pSurface, int nX, int nY);

	virtual void OnMouseEnter(int nX, int nY);
	virtual bool OnMouseMove(int nX, int nY, int nDX, int nDY);
	virtual void OnMouseLeave();
	virtual bool OnMouseButtonDown(int nButton);
	virtual bool OnMouseButtonUp(int nButton);

protected:
	bool m_bDragging;
};

#endif
