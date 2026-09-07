//---------------------------------------------------------------------------
// djItem.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJITEM_H_
#define _DJITEM_H_

#include <SDL3/SDL.h>
#include "djGraph.h"
#include "djUtils.h"
#include <vector>
#include <string>
#include <string_view>

class CdjItem
{
public:
	CdjItem();
	virtual ~CdjItem();

	virtual bool Initialize() { return true; }
	virtual bool Destroy() { return true; }
	virtual bool OnEvent(SDL_Event&, SDL_Keymod) { return false; }
	virtual bool HandleEvent(SDL_Event &Event, SDL_Keymod ModState);

	virtual bool Update(float dt);
	virtual bool UpdateAfter(float dt);

	virtual void Draw(SDL_Renderer*, int, int) {}
	virtual void OnMouseEnter(int, int) {}
	virtual bool OnMouseMove(int, int, int, int) { return false; }
	virtual void OnMouseLeave() {}
	virtual bool OnMouseButtonDown(int) { return false; }
	virtual bool OnMouseButtonUp(int) { return false; }

	bool UpdateAll(float dt);
	void DestroyAll();
	void DrawAll(SDL_Renderer *pRenderer, int nX, int nY);

	virtual void AddChild(CdjItem *pChild);
	size_t GetNumChildren() const noexcept { return m_apChildren.size(); }
	CdjItem *GetChild(size_t uIndex) const { return m_apChildren[uIndex]; }
	void RemoveChild(CdjItem *pItem);

	CdjItem *GetParent() const noexcept { return m_pParent; }

	void SetPos(int x, int y) noexcept { m_nX = x; m_nY = y; }
	void SetSize(int w, int h) noexcept { m_nW = w; m_nH = h; }
	void SetExtents(int x, int y, int w, int h) noexcept { m_nX = x; m_nY = y; m_nW = w; m_nH = h; }

	int m_nX{0};
	int m_nY{0};
	int m_nW{0};
	int m_nH{0};

	const char *GetName() const noexcept { return m_sName.c_str(); }
	void SetName(std::string_view sName) { m_sName = sName; }

	bool m_bMouseInside{false};

protected:
	CdjItem *m_pParent{nullptr};
	std::vector<CdjItem*> m_apChildren;
	std::string m_sName;
};

bool UpdateMouseAll(CdjItem *pItem, int nX = 0, int nY = 0);
bool UpdateMouseButtonDownAll(CdjItem *pItem, int nButton, int nX = 0, int nY = 0);
void UpdateMouseButtonUpAll(CdjItem *pItem, int nButton, int nX = 0, int nY = 0);

class CdjWindow : public CdjItem
{
public:
	CdjWindow();
	~CdjWindow() override = default;

	void AddChild(CdjItem *pChild) override;
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;

	void OnMouseEnter(int nX, int nY) override;
	bool OnMouseMove(int nX, int nY, int nDX, int nDY) override;
	void OnMouseLeave() override;
	bool OnMouseButtonDown(int nButton) override;
	bool OnMouseButtonUp(int nButton) override;

protected:
	bool m_bDragging{false};
};

#endif
