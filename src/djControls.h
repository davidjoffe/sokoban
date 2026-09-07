//---------------------------------------------------------------------------
// djControls.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJCONTROLS_H_
#define _DJCONTROLS_H_

#include "djItem.h"
#include <string>
#include <string_view>
#include <vector>

class CdjButton : public CdjItem
{
public:
	CdjButton() = default;
	~CdjButton() override = default;

	void SetText(std::string_view sNormal, std::string_view sHighlighted = {}, std::string_view sSelected = {});

	bool OnEvent(SDL_Event &Event, SDL_Keymod ModState) override;
	void OnMouseEnter(int, int) override { m_nState = 1; }
	void OnMouseLeave() override { m_nState = 0; }
	bool OnMouseButtonDown(int) override { m_nState = 2; return true; }
	bool OnMouseButtonUp(int) override
	{
		if (m_nState == 2) m_bPressed = true;
		m_nState = 1;
		return true;
	}
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;

	bool WasPressed() noexcept
	{
		if (m_bPressed) { m_bPressed = false; return true; }
		return false;
	}
	void SetFocus(bool bFocus) noexcept { m_bFocus = bFocus; }

protected:
	std::string m_sNormal;
	std::string m_sHighlighted;
	std::string m_sSelected;
	int m_nState{0};
	bool m_bPressed{false};
	bool m_bFocus{false};
};

class djTextEdit : public CdjItem
{
public:
	djTextEdit() = default;
	~djTextEdit() override = default;

	void SetText(std::string_view sLabel, std::string_view sDefaultValue);

	bool OnEvent(SDL_Event &Event, SDL_Keymod ModState) override;
	bool Update(float dt) override;
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;

	bool IsConfirmed() const noexcept { return m_bConfirmed; }
	const char *GetText() const noexcept { return m_sText.c_str(); }

protected:
	void InsertChar(char c);

	std::string m_sLabel;
	std::string m_sText;
	size_t m_nPos{0};
	size_t m_uMaxLen{40};
	float m_fBlinkTimer{0.0f};
	bool m_bConfirmed{false};
};

class djList : public CdjItem
{
public:
	djList() = default;
	~djList() override = default;

	bool OnEvent(SDL_Event &Event, SDL_Keymod ModState) override;
	void OnMouseEnter(int nX, int nY) override;
	bool OnMouseMove(int nX, int nY, int nDX, int nDY) override;
	void OnMouseLeave() override;
	bool OnMouseButtonDown(int nButton) override;
	bool OnMouseButtonUp(int nButton) override;
	bool Update(float dt) override;
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;

	void AddItem(std::string_view sItem);
	void SetItem(size_t nRow, size_t nColumn, std::string_view sItem);

	bool SelectionChanged() noexcept;
	void SetSelection(int nSelected) noexcept { m_nSelected = nSelected; }
	int GetSelection() const noexcept { return m_nSelected; }

protected:
	struct tsItem
	{
		std::vector<std::string> asText;
	};
	std::vector<tsItem> m_aItems;

	int m_nMX{-1};
	int m_nMY{-1};
	int m_nMax{-1};

	int m_nScroll1X{-1};
	int m_nScroll1Y{-1};
	int m_nScroll1W{-1};
	int m_nScroll1H{-1};

	int m_nScroll2X{-1};
	int m_nScroll2Y{-1};
	int m_nScroll2W{-1};
	int m_nScroll2H{-1};

	int m_nSelected{0};
	int m_nOffset{0};
	bool m_bSelectionChanged{false};
};

class djMessageBox : public CdjItem
{
public:
	explicit djMessageBox(std::string_view sText);
	~djMessageBox() override = default;

	bool Initialize() override;
	bool Update(float dt) override;
	bool OnEvent(SDL_Event &Event, SDL_Keymod ModState) override;
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;

	bool m_bClose{false};
	int m_nRet{0};

protected:
	std::string m_sText;
	CdjButton *m_pOK{nullptr};
	CdjButton *m_pCancel{nullptr};
};

#endif
