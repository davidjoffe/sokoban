//---------------------------------------------------------------------------
// Sokoban.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "SDL.h"
#include "Sokoban.h"
#include "djControls.h"
#include "djDesktop.h"
#include "djSettings.h"
#include "GameRegistry.h"
#include <fstream>
#include <sstream>

using namespace nsSokoban;

//___________________________________________________________________________
//
// CChooseLevels
//___________________________________________________________________________

class nsSokoban::CChooseLevels : public CdjItem
{
public:
	CChooseLevels(CLevelManager *pLevelManager, int nCurrentLevel);
	~CChooseLevels() override = default;

	bool Initialize() override;
	bool Destroy() override;
	bool OnEvent(SDL_Event &Event, SDLMod ModState) override;
	bool Update(float dt) override;
	void Draw(SDL_Surface *pSurface, int nX, int nY) override;

	bool m_bClose{false};
	int m_nChosen{-1};

protected:
	int m_nCurrentLevel;
	CLevelManager *m_pLevelManager;
	djList *m_pList{nullptr};
	CdjButton *m_pOK{nullptr};
	CdjButton *m_pCancel{nullptr};
};

CChooseLevels::CChooseLevels(CLevelManager *pLevelManager, int nCurrentLevel)
	: m_nCurrentLevel(nCurrentLevel), m_pLevelManager(pLevelManager)
{
}

bool CChooseLevels::Initialize()
{
	CdjItem::Initialize();

	m_pList = new djList;
	int nListW = (m_nW - 20) + 2;
	int nListH = (m_nH + 2) / 2 + 25;
	m_pList->SetExtents(9, 9, nListW, nListH);

	for (size_t i = 0; i < m_pLevelManager->GetNumLevelSetDescriptors(); ++i)
	{
		const auto &LS = m_pLevelManager->GetLevelSetDescriptor(i);
		m_pList->AddItem(LS.sName);
		m_pList->SetItem(i, 1, LS.sDifficulty);
		m_pList->SetItem(i, 2, LS.sAuthor);
	}
	m_pList->SetSelection(m_nCurrentLevel);
	m_pList->Initialize();
	AddChild(m_pList);

	int nButtonW = (m_nW - (8 + 19 + 19)) / 2;
	m_pOK = new CdjButton;
	m_pOK->SetName("OK");
	m_pOK->Initialize();
	m_pOK->SetText("OK", "OK", "OK");
	m_pOK->SetExtents(19, nListH + 166, nButtonW, 30);
	m_pOK->SetFocus(true);
	AddChild(m_pOK);

	m_pCancel = new CdjButton;
	m_pCancel->SetName("Cancel");
	m_pCancel->Initialize();
	m_pCancel->SetText("Cancel", "Cancel", "Cancel");
	m_pCancel->SetExtents(19 + nButtonW + 9, nListH + 166, nButtonW, 30);
	AddChild(m_pCancel);

	return true;
}

bool CChooseLevels::Destroy()
{
	return CdjItem::Destroy();
}

bool CChooseLevels::OnEvent(SDL_Event &Event, SDLMod ModState)
{
	return CdjItem::OnEvent(Event, ModState);
}

bool CChooseLevels::Update(float dt)
{
	if (m_pCancel && m_pCancel->WasPressed())
	{
		m_bClose = true;
		m_nChosen = -1;
	}
	if (m_pOK && m_pOK->WasPressed())
	{
		m_bClose = true;
		m_nChosen = m_pList->GetSelection();
	}
	return CdjItem::Update(dt);
}

void CChooseLevels::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	djDrawGradientVert(pSurface, nX, nY, m_nW, m_nH, 50, 50, 50, 140, 140, 140, 5);
	djDrawRect(pSurface, nX, nY, m_nW, m_nH, SDL_MapRGB(pSurface->format, 250, 250, 250));

	CdjFont *pFontH = g_pScreen->m_pFont8x13B;
	CdjFont *pFontT = g_pScreen->m_pFont8x13;
	int n = m_pList->GetSelection();
	if (n >= 0 && static_cast<size_t>(n) < m_pLevelManager->GetNumLevelSetDescriptors())
	{
		int nTextX = nX + 10;
		int nTextY = nY + m_nH / 2 + 50;
		const auto &LS = m_pLevelManager->GetLevelSetDescriptor(static_cast<size_t>(n));

		std::string sName = LS.sName;
		if (!LS.sDate.empty())
		{
			sName += " (" + LS.sDate + ")";
		}
		pFontH->DrawText(pSurface, nTextX, nTextY, "Name:");             nTextY += pFontH->GetH();
		pFontT->DrawText(pSurface, nTextX, nTextY, sName.c_str());       nTextY += pFontT->GetH();
		pFontH->DrawText(pSurface, nTextX, nTextY, "Author:");           nTextY += pFontH->GetH();
		pFontT->DrawText(pSurface, nTextX, nTextY, LS.sAuthor.c_str());  nTextY += pFontT->GetH();
		pFontH->DrawText(pSurface, nTextX, nTextY, "URL:");              nTextY += pFontH->GetH();
		pFontT->DrawText(pSurface, nTextX, nTextY, LS.sURL.c_str());     nTextY += pFontT->GetH();
		pFontH->DrawText(pSurface, nTextX, nTextY, "E-Mail:");           nTextY += pFontH->GetH();
		pFontT->DrawText(pSurface, nTextX, nTextY, LS.sEmail.c_str());   nTextY += pFontT->GetH();
		pFontH->DrawText(pSurface, nTextX, nTextY, "Difficulty:");       nTextY += pFontH->GetH();
		pFontT->DrawText(pSurface, nTextX, nTextY, LS.sDifficulty.c_str()); nTextY += pFontT->GetH();
	}

	CdjItem::Draw(pSurface, nX, nY);
}

//___________________________________________________________________________

constexpr int RIGHTBAR = 140;
constexpr float AUTO_FOLLOWMOVETIME = 0.028f;
constexpr float PUSH_SPRITE_TIME = 0.25f;
constexpr int BADMOVE = (LEVEL_MAX_W * LEVEL_MAX_H);

constexpr const char* SETTING_CURRENTLEVELS = "Sokoban-CurrentLevels";
constexpr const char* SETTING_SHADOWS       = "Sokoban-Shadows";

void CSokoban::TryConvertVersion1Settings()
{
	CdjSettings Settings;
	const char *home = std::getenv("HOME");
	std::string sPath = home ? home : ".";
	djAppendPathS(sPath, ".syasokoban1.00");

	if (Settings.LoadSettings(sPath.c_str()))
	{
		const char *sz = Settings.FindSetting("Sokoban1.00-LevelsCompleted");
		if (sz && sz[0] != '\0')
		{
			g_Settings.SetSetting("Sokoban-LevelsCompleted[default.txt]", sz);
		}
	}
}

CSokoban::CSokoban()
{
	g_Settings.SetDefaultSettingBool(SETTING_SHADOWS, true);
	g_Settings.SetDefaultSetting(SETTING_CURRENTLEVELS, "default.txt");
	if (g_Settings.FindSetting("Sokoban-LevelsCompleted[default.txt]") == nullptr)
	{
		TryConvertVersion1Settings();
	}

	m_bShadows = g_Settings.GetSettingBool(SETTING_SHADOWS);
	const char *curSet = g_Settings.FindSetting(SETTING_CURRENTLEVELS);
	m_sCurSet = curSet ? curSet : "default.txt";
	ClearAutoPath();
}

CSokoban::~CSokoban()
{
	delete m_pLevelManager;
	delete m_pCurSet;
	if (m_pSokobanHeader) SDL_FreeSurface(m_pSokobanHeader);
	if (m_pAbout) SDL_FreeSurface(m_pAbout);
	ClearTileSets();
}

void CSokoban::ClearAutoPath()
{
	for (int i = 0; i < LEVEL_MAX_H; ++i)
	{
		for (int j = 0; j < LEVEL_MAX_W; ++j)
		{
			m_FindMap[j][i] = BADMOVE;
		}
	}
	m_aAutoPath.clear();
	m_bBusyFollowingAutoPath = false;
	m_fAutoPathFollowTimer = 0.0f;
}

void CSokoban::FindTarget(int nX, int nY, int nPathLen)
{
	if (nX < 0 || nY < 0 || nX >= m_nLevelW || nY >= m_nLevelH)
		return;
	if (m_CurLevel[nX][nY] != sokBLANK && m_CurLevel[nX][nY] != sokCRATEDEST)
		return;
	if (m_FindMap[nX][nY] <= nPathLen)
		return;

	m_FindMap[nX][nY] = nPathLen++;

	if ((nX == m_nHeroX) && (nY == m_nHeroY))
		return;

	FindTarget(nX - 1, nY, nPathLen);
	FindTarget(nX + 1, nY, nPathLen);
	FindTarget(nX, nY - 1, nPathLen);
	FindTarget(nX, nY + 1, nPathLen);
}

bool CSokoban::Initialize()
{
	SDL_EnableKeyRepeat(250, SDL_DEFAULT_REPEAT_INTERVAL - 5);

	LoadTileSets("default");
	if (m_apTileSets.empty())
		return false;

	SDL_Surface *pSurface = SDL_LoadBMP((GetDataPath() + "sokoban/themes/default/sokoban.bmp").c_str());
	if (pSurface)
	{
		m_pSokobanHeader = SDL_DisplayFormat(pSurface);
		SDL_FreeSurface(pSurface);
	}

	m_pLevelManager = new CLevelManager;
	m_pLevelManager->FindLevelSets(GetDataPath() + "sokoban/levels/");

	if (!LoadLevelset(m_sCurSet.c_str()))
	{
		m_sCurSet = "default.txt";
		LoadLevelset(m_sCurSet.c_str());
	}

	SetName("Sokoban");
	return true;
}

bool CSokoban::Destroy()
{
	delete m_pCurSet;
	m_pCurSet = nullptr;

	delete m_pLevelManager;
	m_pLevelManager = nullptr;

	if (m_pSokobanHeader)
	{
		SDL_FreeSurface(m_pSokobanHeader);
		m_pSokobanHeader = nullptr;
	}
	ClearTileSets();
	return true;
}

void CSokoban::ShowCloseConfirm()
{
	m_pCloseConfirm = new djMessageBox("CLOSE: Are you sure?");
	int nMidX = (m_nW - RIGHTBAR) / 2;
	int nMidY = (m_nH - 16) / 2;
	int nW = (m_nW - RIGHTBAR) / 2;
	int nH = (m_nH - 16) / 6;
	m_pCloseConfirm->SetExtents(nMidX - nW / 2, nMidY - nH / 2, nW, nH);
	m_pCloseConfirm->Initialize();
	AddChild(m_pCloseConfirm);
}

void CSokoban::HideCloseConfirm()
{
	if (m_pCloseConfirm)
	{
		m_pCloseConfirm->Destroy();
		RemoveChild(m_pCloseConfirm);
		delete m_pCloseConfirm;
		m_pCloseConfirm = nullptr;
		g_bMustRedraw.store(true, std::memory_order_relaxed);
	}
}

bool CSokoban::Update(float dt)
{
	if (m_bWon)
	{
		ClearAutoPath();
		if (m_bGotMouseClick)
		{
			SetLevel((m_iCurLev + 1) % GetNumLevels());
		}
	}

	if (m_pCloseConfirm)
	{
		if (m_pCloseConfirm->m_nRet != 0)
		{
			if (m_pCloseConfirm->m_nRet > 0)
			{
				g_pScreen->m_bRunning.store(false, std::memory_order_relaxed);
			}
			HideCloseConfirm();
		}
		return true;
	}

	if (m_pChooseLevels)
	{
		if (m_pChooseLevels->m_bClose)
		{
			int nSelect = m_pChooseLevels->m_nChosen;
			if (nSelect >= 0 && static_cast<size_t>(nSelect) < m_pLevelManager->GetNumLevelSetDescriptors())
			{
				m_sCurSet = m_pLevelManager->GetLevelSetDescriptor(static_cast<size_t>(nSelect)).sLevelFile;
				LoadLevelset(m_sCurSet.c_str());
				g_Settings.SetSetting(SETTING_CURRENTLEVELS, m_sCurSet);
			}
			CloseChooseLevels();
		}
		return true;
	}

	if (m_bShowingAbout)
	{
		if (!m_pAbout)
			m_bShowingAbout = false;
		return true;
	}

	if (!m_pTileSet)
		return false;

	int nSpriteSize = m_pTileSet->nW;
	bool bLeftMouseDown = (g_pScreen->m_nMouseButtons & 1) != 0;

	if (!m_bBusyFollowingAutoPath)
	{
		m_bMayPickCrates = true;
	}
	if (!bLeftMouseDown)
	{
		m_nCrateX = -1;
		m_nCrateY = -1;
	}

	if (!m_bWon && m_bMouseInside && bLeftMouseDown)
	{
		if (m_nCrateX == -1 || m_nCrateY == -1)
		{
			if (!m_bBusyFollowingAutoPath)
			{
				if (!m_aAutoPath.empty())
				{
					m_bMayPickCrates = false;
					m_bBusyFollowingAutoPath = true;
					m_fAutoPathFollowTimer = AUTO_FOLLOWMOVETIME;
				}
				else
					m_nHeroDirection = 0;
			}
		}
	}

	if (m_bBusyFollowingAutoPath)
	{
		m_fAutoPathFollowTimer -= dt;
		while (m_fAutoPathFollowTimer < 0.0f && m_bBusyFollowingAutoPath)
		{
			m_fAutoPathFollowTimer += AUTO_FOLLOWMOVETIME;
			TryMove(m_aAutoPath[0].dx, m_aAutoPath[0].dy);
			g_bMustRedraw.store(true, std::memory_order_relaxed);
			if (!m_aAutoPath.empty())
				m_aAutoPath.erase(m_aAutoPath.begin());
			if (m_aAutoPath.empty())
			{
				ClearAutoPath();
				m_nHeroDirection = 0;
			}
		}
	}

	if (HaveLevel())
	{
		int iOffsetX = ((m_nW - RIGHTBAR) / 2) - ((m_nLevelW * nSpriteSize) / 2);
		int iOffsetY = (m_nH / 2) - ((m_nLevelH * nSpriteSize) / 2);
		if (iOffsetX < 0) iOffsetX = 0;
		if (iOffsetY < 0) iOffsetY = 0;

		if (!m_bWon && m_bMouseInside
			&& m_nMouseX >= iOffsetX && m_nMouseY >= iOffsetY
			&& m_nMouseX < iOffsetX + m_nLevelW * nSpriteSize && m_nMouseY < iOffsetY + m_nLevelH * nSpriteSize)
		{
			int nBlockX = (m_nMouseX - iOffsetX) / nSpriteSize;
			int nBlockY = (m_nMouseY - iOffsetY) / nSpriteSize;
			bool bMoved = (nBlockX != m_nTargetX || nBlockY != m_nTargetY);
			m_nTargetX = nBlockX;
			m_nTargetY = nBlockY;

			if (m_bMayPickCrates && IsCrate(m_CurLevel[nBlockX][nBlockY]))
			{
				ClearAutoPath();
				if ((m_nHeroX == nBlockX && djABS(nBlockY - m_nHeroY) == 1) ||
					(m_nHeroY == nBlockY && djABS(nBlockX - m_nHeroX) == 1))
				{
					if (bLeftMouseDown && m_nCrateX == -1 && m_nCrateY == -1)
					{
						m_nCrateX = nBlockX;
						m_nCrateY = nBlockY;
					}
				}
			}
			else
			{
				if (!m_bBusyFollowingAutoPath && bMoved)
				{
					ClearAutoPath();
					FindTarget(m_nTargetX, m_nTargetY, 0);
					if (m_FindMap[m_nHeroX][m_nHeroY] != BADMOVE)
					{
						int cx = m_nHeroX;
						int cy = m_nHeroY;
						while (m_FindMap[cx][cy] > 0)
						{
							SAutoMove Move{0, 0};
							if (cx > 0 && m_FindMap[cx - 1][cy] == (m_FindMap[cx][cy] - 1))
							{
								Move.dx = -1; m_aAutoPath.push_back(Move); cx--;
							}
							else if (cx + 1 < LEVEL_MAX_W && m_FindMap[cx + 1][cy] == (m_FindMap[cx][cy] - 1))
							{
								Move.dx = 1; m_aAutoPath.push_back(Move); cx++;
							}
							else if (cy > 0 && m_FindMap[cx][cy - 1] == (m_FindMap[cx][cy] - 1))
							{
								Move.dy = -1; m_aAutoPath.push_back(Move); cy--;
							}
							else if (cy + 1 < LEVEL_MAX_H && m_FindMap[cx][cy + 1] == (m_FindMap[cx][cy] - 1))
							{
								Move.dy = 1; m_aAutoPath.push_back(Move); cy++;
							}
							else
							{
								break;
							}
						}
					}
				}
			}

			if (m_nCrateX != -1 && m_nCrateY != -1)
			{
				if (nBlockX != m_nCrateX || nBlockY != m_nCrateY)
				{
					int nDiffX = nBlockX - m_nCrateX;
					int nDiffY = nBlockY - m_nCrateY;
					if (nDiffX == 0 || nDiffY == 0)
					{
						int nDX = djMIN(1, djMAX(-1, nDiffX));
						int nDY = djMIN(1, djMAX(-1, nDiffY));
						bool bCanMove = true;
						while (bCanMove && (m_nCrateX != nBlockX || m_nCrateY != nBlockY))
						{
							int nHeroX = m_nHeroX;
							int nHeroY = m_nHeroY;
							if (m_nHeroX + nDX == m_nCrateX && m_nHeroY + nDY == m_nCrateY)
							{
								TryMove(nDX, nDY);
							}
							if (nHeroX == m_nHeroX && nHeroY == m_nHeroY)
							{
								bCanMove = false;
							}
							else
							{
								m_nCrateX = m_nHeroX + nDX;
								m_nCrateY = m_nHeroY + nDY;
							}
						}
					}
				}
			}
		}
		else
		{
			m_nTargetX = -1;
			m_nTargetY = -1;
			if (!m_bBusyFollowingAutoPath)
				ClearAutoPath();
		}
	}

	if (m_fPushedTimer > 0.0f)
	{
		m_fPushedTimer -= dt;
		if (m_fPushedTimer <= 0.0f)
			g_bMustRedraw.store(true, std::memory_order_relaxed);
	}

	return true;
}

void CSokoban::Draw(SDL_Surface *pSurface, int nX, int nY)
{
	CdjFont *pFont = g_pScreen->m_pFont8x13B;

	if (m_bShowingAbout)
	{
		if (m_pAbout)
			djBlit(pSurface, m_pAbout, m_nW / 2 - 320 / 2, m_nH / 2 - 240 / 2);
		return;
	}

	if (!HaveLevel())
		pFont->DrawText(pSurface, nX + 20, nY + 20, "NO LEVEL");

	if (!m_pTileSet)
	{
		nY += pFont->GetH();
		pFont->DrawText(pSurface, nX + 20, nY + 20, "NO TILESET");
		return;
	}

	if (HaveLevel())
	{
		int nSpriteSize = m_pTileSet->nW;
		SDL_Surface *pSprites = m_pTileSet->pSprites;

		int iOffsetX = ((m_nW - RIGHTBAR) / 2) - ((m_nLevelW * nSpriteSize) / 2);
		int iOffsetY = (m_nH / 2) - ((m_nLevelH * nSpriteSize) / 2);
		if (iOffsetX < 0) iOffsetX = 0;
		if (iOffsetY < 0) iOffsetY = 0;

		int nBorderWidth = nSpriteSize / 2;

		djDrawSprite(0, pSurface, m_pTileSet->pBorders, nX + iOffsetX - nBorderWidth, nY + iOffsetY - nBorderWidth, nBorderWidth, nBorderWidth);
		djDrawSprite(1, pSurface, m_pTileSet->pBorders, nX + iOffsetX + m_nLevelW * nSpriteSize, nY + iOffsetY - nBorderWidth, nBorderWidth, nBorderWidth);
		djDrawSprite(5, pSurface, m_pTileSet->pBorders, nX + iOffsetX + m_nLevelW * nSpriteSize, nY + iOffsetY + m_nLevelH * nSpriteSize, nBorderWidth, nBorderWidth);
		djDrawSprite(4, pSurface, m_pTileSet->pBorders, nX + iOffsetX - nBorderWidth, nY + iOffsetY + m_nLevelH * nSpriteSize, nBorderWidth, nBorderWidth);

		for (int i = 0; i < m_nLevelW; ++i)
		{
			djDrawSprite(1, pSurface, m_pTileSet->pBorders, nX + iOffsetX + i * nSpriteSize, nY + iOffsetY - nBorderWidth, nSpriteSize, nBorderWidth);
			djDrawSprite(3, pSurface, m_pTileSet->pBorders, nX + iOffsetX + i * nSpriteSize, nY + iOffsetY + m_nLevelH * nSpriteSize, nSpriteSize, nBorderWidth);
		}
		for (int i = 0; i < m_nLevelH; ++i)
		{
			djDrawSprite(4, pSurface, m_pTileSet->pBorders, nX + iOffsetX - nBorderWidth, nY + iOffsetY + i * nSpriteSize, nBorderWidth, nSpriteSize);
			djDrawSprite(5, pSurface, m_pTileSet->pBorders, nX + iOffsetX + m_nLevelW * nSpriteSize, nY + iOffsetY + i * nSpriteSize, nBorderWidth, nSpriteSize);
		}

		int nOffsetY = iOffsetY + nY;
		for (int i = 0; i < m_nLevelH; ++i)
		{
			int nCurrX = iOffsetX + nX;
			for (int j = 0; j < m_nLevelW; ++j)
			{
				char cLevChar = m_CurLevel[j][i];
				int nShadowBits = 0;

				if (m_bShadows)
				{
					if (i > 0 && CastsShadow(m_CurLevel[j][i - 1])) nShadowBits |= 1;
					if (j > 0 && CastsShadow(m_CurLevel[j - 1][i])) nShadowBits |= 4;
					if (nShadowBits != 5 && i > 0 && j > 0 && CastsShadow(m_CurLevel[j - 1][i - 1]))
						nShadowBits |= 2;
				}

				switch (cLevChar)
				{
				case sokBLANK:
					djDrawSprite(sokBLANK + nShadowBits, pSurface, pSprites, nCurrX, nOffsetY, nSpriteSize, nSpriteSize);
					break;
				case sokCRATEDEST:
					djDrawSprite(sokCRATEDEST + nShadowBits, pSurface, pSprites, nCurrX, nOffsetY, nSpriteSize, nSpriteSize);
					break;
				default:
					djDrawSprite(cLevChar, pSurface, pSprites, nCurrX, nOffsetY, nSpriteSize, nSpriteSize);
				}
				nCurrX += nSpriteSize;
			}
			nOffsetY += nSpriteSize;
		}

		if (!m_bWon && m_bMouseInside
			&& m_nMouseX >= iOffsetX && m_nMouseY >= iOffsetY
			&& m_nMouseX < iOffsetX + m_nLevelW * nSpriteSize && m_nMouseY < iOffsetY + m_nLevelH * nSpriteSize)
		{
			if (m_nCrateX != -1 && m_nCrateY != -1)
			{
				djDrawRect(pSurface, nX + iOffsetX + m_nCrateX * nSpriteSize, nY + iOffsetY + m_nCrateY * nSpriteSize, nSpriteSize, nSpriteSize, SDL_MapRGB(pSurface->format, 120, 120, 255));
			}
			else
			{
				int nBlockX = (m_nMouseX - iOffsetX) / nSpriteSize;
				int nBlockY = (m_nMouseY - iOffsetY) / nSpriteSize;
				if (m_bMayPickCrates && IsCrate(m_CurLevel[nBlockX][nBlockY]))
				{
					if ((m_nHeroX == nBlockX && djABS(nBlockY - m_nHeroY) == 1) ||
						(m_nHeroY == nBlockY && djABS(nBlockX - m_nHeroX) == 1))
					{
						djDrawRect(pSurface, nX + iOffsetX + nBlockX * nSpriteSize, nY + iOffsetY + nBlockY * nSpriteSize, nSpriteSize, nSpriteSize, SDL_MapRGB(pSurface->format, 70, 70, 200));
					}
				}
			}
		}

		if (!m_aAutoPath.empty())
		{
			if (m_bBusyFollowingAutoPath)
			{
				if (m_aAutoPath.size() > 3)
				{
					int nPathX = m_nHeroX;
					int nPathY = m_nHeroY;
					for (size_t i = 0; i < m_aAutoPath.size(); ++i)
					{
						int x1 = nX + iOffsetX + nPathX * nSpriteSize + nSpriteSize / 2;
						int y1 = nY + iOffsetY + nPathY * nSpriteSize + nSpriteSize / 2;
						nPathX += m_aAutoPath[i].dx;
						nPathY += m_aAutoPath[i].dy;
						int x2 = nX + iOffsetX + nPathX * nSpriteSize + nSpriteSize / 2;
						int y2 = nY + iOffsetY + nPathY * nSpriteSize + nSpriteSize / 2;
						djDrawRect(pSurface,
							djMIN(x1, x2),
							djMIN(y1, y2),
							m_aAutoPath[i].dx != 0 ? nSpriteSize + 1 : 2,
							m_aAutoPath[i].dy != 0 ? nSpriteSize + 1 : 2,
							SDL_MapRGB(pSurface->format, djMIN(255, static_cast<int>(100 + i * 25)), 255, djMIN(255, static_cast<int>(60 + i * 10))));
					}
				}
			}
			else
			{
				djDrawRect(pSurface, nX + iOffsetX + m_nTargetX * nSpriteSize, nY + iOffsetY + m_nTargetY * nSpriteSize, nSpriteSize, nSpriteSize, SDL_MapRGB(pSurface->format, 80, 255, 80));
			}
		}

		int heroSprite = m_bWon ? 40 : (m_fPushedTimer > 0.0f ? 40 : 32) + m_nHeroDirection;
		djDrawSprite(heroSprite, pSurface, pSprites, nX + iOffsetX + m_nHeroX * nSpriteSize, nY + iOffsetY + m_nHeroY * nSpriteSize, nSpriteSize, nSpriteSize);
	}

	//
	// RIGHT BAR
	//

	// Sokoban header in top right
	if (m_pSokobanHeader)
	{
		int headerX = nX + m_nW - RIGHTBAR;
		int headerY = nY;
		djBlit(pSurface, m_pSokobanHeader, headerX, headerY);

		// Renderiza a versão dinamicamente a partir do registro de jogos
		std::string sVer = "version " + g_GameRegistry.GetGameDescriptor(0).sVersion;
		CdjFont *pVerFont = g_pScreen->m_pFont8x13;
		if (pVerFont)
		{
			// Centralizado logo abaixo do logotipo "Sokoban"
			int verX = headerX + (RIGHTBAR - pVerFont->GetStringWidth(sVer.c_str())) / 2;
			pVerFont->DrawText(pSurface, verX, headerY + 48, sVer.c_str());
		}
	}

	// Gray 'divider' separating game area and stuff on right
	djDrawFilledRect(pSurface, nX + m_nW - RIGHTBAR, nY, 1, m_nH, SDL_MapRGB(pSurface->format, 60, 60, 60));

	int nDY = pFont->GetH();
	int nHelpY = nY + m_nH - 11 * nDY * 2;
	int nHelpX = nX + m_nW - RIGHTBAR + 8;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "Alt+Enter:");        nHelpY += nDY;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "Fullscreen Mode");    nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "C: Choose Levels");   nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "M: Next Level");      nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "N: Prev Level");      nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "R: Reset");           nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "U: Undo");            nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "S: Shadows");         nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "A: About");           nHelpY += nDY * 2;
	pFont->DrawText(pSurface, nHelpX, nHelpY, "ESC: Exit");          nHelpY += nDY * 2;

	int nStatusBarHeight = nDY + 10;
	djDrawFilledRect(pSurface, nX, nY + m_nH - nStatusBarHeight,     m_nW, 1, SDL_MapRGB(pSurface->format, 182, 182, 182));
	djDrawFilledRect(pSurface, nX, nY + m_nH - nStatusBarHeight + 1, m_nW, 1, SDL_MapRGB(pSurface->format, 142, 142, 142));
	djDrawFilledRect(pSurface, nX, nY + m_nH - nStatusBarHeight + 2, m_nW, nStatusBarHeight - 2, SDL_MapRGB(pSurface->format, 110, 110, 110));

	if (m_pCurSet && m_iCurLev >= 0)
	{
		std::string sInfo = "Level: " + std::to_string(m_iCurLev + 1) + "/" + std::to_string(GetNumLevels())
			+ " Moves: " + std::to_string(m_iNumMoves) + " Pushes: " + std::to_string(m_iNumPushes);
		pFont->DrawText(pSurface, nX + m_nW - pFont->GetStringWidth(sInfo.c_str()) - 8, nY + m_nH - nDY - 4, sInfo.c_str());
	}

	std::string sTitle = "[" + m_LevelSet.sName;
	if (!m_LevelSet.sAuthor.empty())
		sTitle += " - " + m_LevelSet.sAuthor;
	sTitle += "] ";

	if (m_pCurSet && m_iCurLev >= 0 && m_nMovesTaken != -1)
	{
		sTitle += "Completed (" + std::to_string(m_nMovesTaken) + " move" + (m_nMovesTaken == 1 ? "" : "s") + ")";
	}
	pFont->DrawText(pSurface, nX + 8, nY + m_nH - nDY - 4, sTitle.c_str());

	if (m_bWon && !m_bGotMouseClick)
	{
		const char *msg = "SPACE or mouse click to continue";
		pFont->DrawText(pSurface, nX + (m_nW - RIGHTBAR) / 2 - pFont->GetStringWidth(msg) / 2, nY + m_nH - 24 - pFont->GetH(), msg);
	}
}

bool CSokoban::LoadLevelset(const char *szFilename)
{
	std::string sFilename = GetDataPath() + "sokoban/levels/" + szFilename;
	std::ifstream fin(sFilename);
	if (!fin.is_open())
	{
		ChooseTileSet();
		return false;
	}

	m_sCurSet = szFilename;
	m_LevelSet.Reset();
	m_pLevelManager->FindLevelSetDescriptor(szFilename, m_LevelSet);

	delete m_pCurSet;
	m_pCurSet = new CLevelSet;

	CLevel *pLev = nullptr;
	int ysize = 0;
	int iNumCrates = 0;
	int iNumCrateDests = 0;

	std::string line;
	while (std::getline(fin, line))
	{
		djStripCRLF(line);

		if (line.empty() || line[0] == '~')
		{
			if (pLev && iNumCrates == iNumCrateDests && iNumCrates != 0)
			{
				m_pCurSet->apLevels.push_back(pLev);
				pLev = nullptr;
			}
			else
			{
				delete pLev;
				pLev = nullptr;
			}
			ysize = 0;
			iNumCrates = 0;
			iNumCrateDests = 0;
		}
		else
		{
			while (!line.empty() && line.back() == ' ')
				line.pop_back();

			char c = line.empty() ? '\0' : line[0];
			if (c == ' ' || c == '#' || c == '$' || c == '.' || c == '*' || c == '@' || c == '+')
			{
				if (!pLev)
				{
					pLev = new CLevel;
				}

				for (size_t i = 0; i < line.length(); ++i)
				{
					switch (line[i])
					{
					case ' ': line[i] = sokBLANK; break;
					case '#': line[i] = sokWALL; break;
					case '$': line[i] = sokCRATE; iNumCrates++; break;
					case '.': line[i] = sokCRATEDEST; iNumCrateDests++; break;
					case '+': line[i] = sokCRATEDEST; iNumCrateDests++; pLev->x = static_cast<int>(i); pLev->y = ysize; break;
					case '*': line[i] = sokCRATEDONE; iNumCrates++; iNumCrateDests++; break;
					case '@': line[i] = sokBLANK; pLev->x = static_cast<int>(i); pLev->y = ysize; break;
					default:  line[i] = sokBLANK; break;
					}
				}

				while (!line.empty() && line.back() == sokBLANK)
					line.pop_back();

				if (line.length() > LEVEL_MAX_W)
					line.resize(LEVEL_MAX_W);

				if (!line.empty() && pLev->apLines.size() < LEVEL_MAX_H)
				{
					pLev->apLines.push_back(line);
					ysize++;
				}
			}
		}
	}

	if (pLev && iNumCrates == iNumCrateDests && iNumCrates != 0)
	{
		m_pCurSet->apLevels.push_back(pLev);
	}
	else
	{
		delete pLev;
	}

	int n = 0;
	while (IsLevelCompleted(n) && n < GetNumLevels() - 1)
	{
		n++;
	}
	SetLevel(n);

	return true;
}

int CSokoban::SetLevel(int iLev)
{
	m_nMovesTaken = GetNumMovesTaken(iLev);
	m_aUndo.clear();
	m_iNumMoves = 0;
	m_iNumPushes = 0;
	m_iCurLev = iLev;

	CLevel *pLev = (m_iCurLev >= 0 && m_iCurLev < GetNumLevels()) ? m_pCurSet->apLevels[m_iCurLev] : nullptr;

	for (int i = 0; i < LEVEL_MAX_H; ++i)
	{
		for (int j = 0; j < LEVEL_MAX_W; ++j)
		{
			m_CurLevel[j][i] = sokBLANK;
		}
	}

	if (pLev)
	{
		m_nLevelH = static_cast<int>(pLev->apLines.size());
		m_nLevelW = 0;
		for (int i = 0; i < m_nLevelH; ++i)
		{
			const std::string &line = pLev->apLines[i];
			if (static_cast<int>(line.length()) > m_nLevelW)
				m_nLevelW = static_cast<int>(line.length());
			for (size_t j = 0; j < line.length(); ++j)
			{
				m_CurLevel[j][i] = line[j];
			}
		}
		m_nHeroX = pLev->x;
		m_nHeroY = pLev->y;
	}
	else
	{
		m_nLevelH = 0;
		m_nLevelW = 0;
		m_nHeroX = 0;
		m_nHeroY = 0;
	}

	for (int i = 0; i < LEVEL_MAX_H; ++i)
	{
		for (int j = 0; j < LEVEL_MAX_W; ++j)
		{
			if (IsWall(m_CurLevel[j][i]))
			{
				int nBits = 0;
				if (i >= 1 && IsWall(m_CurLevel[j][i - 1])) nBits |= 1;
				if (j < LEVEL_MAX_W - 1 && IsWall(m_CurLevel[j + 1][i])) nBits |= 2;
				if (i < LEVEL_MAX_H - 1 && IsWall(m_CurLevel[j + 1][i])) nBits |= 4;
				if (j >= 1 && IsWall(m_CurLevel[j - 1][i])) nBits |= 8;
				m_CurLevel[j][i] += nBits;
				if (m_CurLevel[j][i] == sokWALL + 5 && (rand() % 35) == 0)
					m_CurLevel[j][i] = sokWALL_CRACKED;
			}
		}
	}

	ClearAutoPath();
	m_bMayPickCrates = true;
	m_nCrateX = -1;
	m_nCrateY = -1;
	m_nHeroDirection = 0;
	m_fPushedTimer = 0.0f;
	m_bWon = false;
	m_bGotMouseClick = false;

	ChooseTileSet();
	return 0;
}

void CSokoban::MoveHero(int dx, int dy)
{
	m_nHeroX += dx;
	m_nHeroY += dy;
	m_iNumMoves++;

	if (!m_bBusyFollowingAutoPath)
	{
		m_nTargetX = -1;
		m_nTargetY = -1;
		ClearAutoPath();
	}
}

void CSokoban::SetHeroDirection(int dx, int dy)
{
	if (dx < 0) m_nHeroDirection = 4;
	else if (dx > 0) m_nHeroDirection = 2;
	else if (dy < 0) m_nHeroDirection = 1;
	else if (dy > 0) m_nHeroDirection = 3;
	else m_nHeroDirection = 0;
}

void CSokoban::SetShowingAbout(bool b)
{
	if (!b)
	{
		m_bShowingAbout = false;
		if (m_pAbout) SDL_FreeSurface(m_pAbout);
		m_pAbout = nullptr;
	}
	else
	{
		SDL_Surface *pSurface = SDL_LoadBMP((GetDataPath() + "sokoban/about.bmp").c_str());
		if (pSurface)
		{
			m_pAbout = SDL_DisplayFormat(pSurface);
			SDL_FreeSurface(pSurface);
			m_bShowingAbout = true;
		}
	}
}

bool CSokoban::OnMouseButtonDown(int nButton)
{
	if (nButton == SDL_BUTTON_LEFT)
	{
		if (m_bShowingAbout)
		{
			SetShowingAbout(false);
		}
		else if (m_bWon && !m_bGotMouseClick)
		{
			m_bGotMouseClick = true;
		}
	}
	return true;
}

bool CSokoban::OnMouseMove(int nX, int nY, int, int)
{
	m_nMouseX = nX;
	m_nMouseY = nY;
	return true;
}

void CSokoban::TryMove(int dx, int dy)
{
	if (!HaveLevel()) return;

	SetHeroDirection(dx, dy);
	if ((m_nHeroX + dx < 0) || (m_nHeroY + dy < 0) || (m_nHeroX + dx >= m_nLevelW) || (m_nHeroY + dy >= m_nLevelH))
		return;

	int iDest = m_CurLevel[m_nHeroX + dx][m_nHeroY + dy];

	switch (iDest)
	{
	case sokBLANK:
	case sokCRATEDEST:
		{
			SUndoMove Undo{};
			Undo.bCrate = false;
			Undo.x = m_nHeroX;
			Undo.y = m_nHeroY;
			Undo.nHeroDirection = m_nHeroDirection;
			m_aUndo.push_back(Undo);
			MoveHero(dx, dy);
		}
		break;

	case sokCRATEDONE:
	case sokCRATE:
		{
			if (m_nHeroX + dx + dx < 0 || m_nHeroY + dy + dy < 0 ||
				m_nHeroX + dx + dx >= m_nLevelW || m_nHeroY + dy + dy >= m_nLevelH)
				return;

			int iNext = m_CurLevel[m_nHeroX + dx + dx][m_nHeroY + dy + dy];
			switch (iNext)
			{
			case sokBLANK:
			case sokCRATEDEST:
				if (!m_bWon)
				{
					SUndoMove Undo{};
					Undo.x = m_nHeroX;
					Undo.y = m_nHeroY;
					Undo.nHeroDirection = m_nHeroDirection;
					Undo.bCrate = true;
					Undo.cxB = m_nHeroX + dx;
					Undo.cyB = m_nHeroY + dy;
					Undo.cxA = m_nHeroX + dx + dx;
					Undo.cyA = m_nHeroY + dy + dy;
					m_aUndo.push_back(Undo);

					m_CurLevel[m_nHeroX + dx + dx][m_nHeroY + dy + dy] = (iNext == sokCRATEDEST ? sokCRATEDONE : sokCRATE);
					m_CurLevel[m_nHeroX + dx][m_nHeroY + dy] = (iDest == sokCRATEDONE ? sokCRATEDEST : sokBLANK);
					MoveHero(dx, dy);
					m_iNumPushes++;
					m_fPushedTimer = PUSH_SPRITE_TIME;
				}
				break;
			default:
				m_fPushedTimer = PUSH_SPRITE_TIME;
				break;
			}
		}
		break;

	default:
		m_fPushedTimer = PUSH_SPRITE_TIME;
		break;
	}

	for (int i = 0; i < m_nLevelH; ++i)
	{
		for (int j = 0; j < m_nLevelW; ++j)
		{
			if (m_CurLevel[j][i] == sokCRATE)
				return;
		}
	}

	m_bWon = true;
	m_bGotMouseClick = false;
	AddCompletedLevel(m_iCurLev);
}

void CSokoban::CloseChooseLevels()
{
	if (m_pChooseLevels)
	{
		m_pChooseLevels->Destroy();
		RemoveChild(m_pChooseLevels);
		delete m_pChooseLevels;
		m_pChooseLevels = nullptr;
		g_bMustRedraw.store(true, std::memory_order_relaxed);
	}
}

bool CSokoban::OnEvent(SDL_Event &Event, SDLMod /*ModState*/)
{
	if (Event.type == SDL_QUIT)
	{
		if (!m_pCloseConfirm && !m_pChooseLevels && !m_bShowingAbout)
		{
			ShowCloseConfirm();
			return true;
		}
	}

	if (Event.type == SDL_KEYDOWN && Event.key.type == SDL_KEYDOWN)
	{
		if (m_pCloseConfirm)
		{
			return false;
		}
		if (m_pChooseLevels)
		{
			if (Event.key.keysym.sym == SDLK_ESCAPE)
			{
				CloseChooseLevels();
				return true;
			}
			return false;
		}
		if (m_bShowingAbout)
		{
			if (Event.key.keysym.sym == SDLK_a || Event.key.keysym.sym == SDLK_ESCAPE || Event.key.keysym.sym == SDLK_SPACE)
			{
				SetShowingAbout(false);
				return true;
			}
			return false;
		}

		switch (Event.key.keysym.sym)
		{
		case SDLK_UP:    if (!m_bBusyFollowingAutoPath) TryMove(0, -1); return true;
		case SDLK_DOWN:  if (!m_bBusyFollowingAutoPath) TryMove(0, 1);  return true;
		case SDLK_LEFT:  if (!m_bBusyFollowingAutoPath) TryMove(-1, 0); return true;
		case SDLK_RIGHT: if (!m_bBusyFollowingAutoPath) TryMove(1, 0);  return true;
		case SDLK_ESCAPE: ShowCloseConfirm(); return true;

		case SDLK_c:
			{
				m_pChooseLevels = new CChooseLevels(m_pLevelManager, m_pLevelManager->FindLevelSetDescriptorIndex(m_sCurSet.c_str()));
				int nW = (m_nW - RIGHTBAR) - 20;
				int nH = (m_nH - 16) - 120;
				m_pChooseLevels->SetExtents(10, 60, nW, nH);
				m_pChooseLevels->Initialize();
				AddChild(m_pChooseLevels);
			}
			return true;

		case SDLK_m: if (HaveLevel() && m_iCurLev < GetNumLevels() - 1) { m_bWon = true; m_bGotMouseClick = true; } return true;
		case SDLK_n: if (HaveLevel() && m_iCurLev > 0) SetLevel(m_iCurLev - 1); return true;
		case SDLK_r: if (HaveLevel()) SetLevel(m_iCurLev); return true;
		case SDLK_u: if (HaveLevel()) UndoMove(); return true;
		case SDLK_s:
			if (HaveLevel())
			{
				m_bShadows = !m_bShadows;
				g_Settings.SetSettingBool(SETTING_SHADOWS, m_bShadows);
			}
			return true;
		case SDLK_a:
			SetShowingAbout(true);
			return true;
		case SDLK_SPACE:
			if (m_bWon && !m_bGotMouseClick)
				m_bGotMouseClick = true;
			return true;
		default:
			break;
		}
	}
	return false;
}

void CSokoban::UndoMove()
{
	if (m_aUndo.empty())
		return;

	SUndoMove Undo = m_aUndo.back();
	m_aUndo.pop_back();

	if (Undo.bCrate)
	{
		int iAfter = m_CurLevel[Undo.cxA][Undo.cyA];
		int iBefore = m_CurLevel[Undo.cxB][Undo.cyB];
		m_CurLevel[Undo.cxA][Undo.cyA] = (iAfter == sokCRATE ? sokBLANK : sokCRATEDEST);
		m_CurLevel[Undo.cxB][Undo.cyB] = (iBefore == sokBLANK ? sokCRATE : sokCRATEDONE);
		m_iNumPushes--;
		m_fPushedTimer = 0.0f;
	}
	m_nHeroX = Undo.x;
	m_nHeroY = Undo.y;
	m_nHeroDirection = Undo.nHeroDirection;
	m_iNumMoves--;
}

std::string CSokoban::GetLevelsCompletedSettingsKey()
{
	return "Sokoban-LevelsCompleted[" + m_sCurSet + "]";
}

bool CSokoban::IsLevelCompleted(int nLevelNum)
{
	const char *szStr = g_Settings.FindSetting(GetLevelsCompletedSettingsKey());
	if (!szStr || szStr[0] == '\0')
		return false;

	std::istringstream iss(szStr);
	std::string token;
	bool bIsLevel = true;
	while (std::getline(iss, token, '.'))
	{
		if (bIsLevel && !token.empty() && std::atoi(token.c_str()) == nLevelNum)
			return true;
		bIsLevel = !bIsLevel;
	}
	return false;
}

int CSokoban::GetNumMovesTaken(int nLevelNum)
{
	const char *szStr = g_Settings.FindSetting(GetLevelsCompletedSettingsKey());
	if (!szStr || szStr[0] == '\0')
		return -1;

	std::istringstream iss(szStr);
	std::string token;
	bool bIsLevel = true;
	int curLevel = -1;
	while (std::getline(iss, token, '.'))
	{
		if (bIsLevel)
		{
			curLevel = token.empty() ? -1 : std::atoi(token.c_str());
		}
		else if (curLevel == nLevelNum)
		{
			return token.empty() ? -1 : std::atoi(token.c_str());
		}
		bIsLevel = !bIsLevel;
	}
	return -1;
}

void CSokoban::AddCompletedLevel(int nLevelNum)
{
	if (IsLevelCompleted(nLevelNum))
	{
		if (m_nMovesTaken > m_iNumMoves)
		{
			std::string newSetting;
			for (int i = 0; i < GetNumLevels(); ++i)
			{
				if (IsLevelCompleted(i))
				{
					int moves = (i == nLevelNum) ? m_iNumMoves : GetNumMovesTaken(i);
					newSetting += std::to_string(i) + "." + std::to_string(moves) + ".";
				}
			}
			g_Settings.SetSetting(GetLevelsCompletedSettingsKey(), newSetting);
			m_nMovesTaken = m_iNumMoves;
		}
		return;
	}

	const char *szStr = g_Settings.FindSetting(GetLevelsCompletedSettingsKey());
	std::string current = szStr ? szStr : "";
	current += std::to_string(nLevelNum) + "." + std::to_string(m_iNumMoves) + ".";

	g_Settings.SetSetting(GetLevelsCompletedSettingsKey(), current);
	CommitSettings();
}

void CSokoban::LoadTileSets(const char *szDirectory)
{
	ClearTileSets();
	for (int i = 1; i < 128; ++i)
	{
		auto *pTileSet = new STileSet;
		if (pTileSet->Load(szDirectory, i))
		{
			m_apTileSets.push_back(pTileSet);
		}
		else
		{
			delete pTileSet;
		}
	}
}

void CSokoban::ClearTileSets()
{
	for (auto *ts : m_apTileSets)
	{
		delete ts;
	}
	m_apTileSets.clear();
	m_pTileSet = nullptr;
}

void CSokoban::STileSet::Destroy()
{
	if (pSprites) SDL_FreeSurface(pSprites);
	if (pBorders) SDL_FreeSurface(pBorders);
	pSprites = nullptr;
	pBorders = nullptr;
	nW = 0;
	nH = 0;
}

void CSokoban::ChooseTileSet()
{
	m_pTileSet = nullptr;
	if (m_apTileSets.empty())
		return;

	if (m_nLevelW > 0 && m_nLevelH > 0)
	{
		int nMaxTileW = ((m_nW - RIGHTBAR) - 16) / m_nLevelW;
		int nMaxTileH = ((m_nH - 16) - 16) / m_nLevelH;
		for (auto *ts : m_apTileSets)
		{
			if (ts->nW <= nMaxTileW && ts->nH <= nMaxTileH)
				m_pTileSet = ts;
		}
	}
	if (!m_pTileSet && !m_apTileSets.empty())
		m_pTileSet = m_apTileSets[0];
}

bool CSokoban::STileSet::Load(const char *szDirectory, int nSize)
{
	std::string sBase = GetDataPath() + "sokoban/themes/" + szDirectory + "/";
	nW = nSize;
	nH = nSize;

	std::string sSprites = sBase + "sprites" + std::to_string(nSize) + ".bmp";
	SDL_Surface *pSurface = SDL_LoadBMP(sSprites.c_str());
	if (!pSurface)
	{
		Destroy();
		return false;
	}

	SDL_SetColorKey(pSurface, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(pSurface->format, 0, 255, 0));
	pSprites = SDL_DisplayFormat(pSurface);
	SDL_FreeSurface(pSurface);

	std::string sBorders = sBase + "borders" + std::to_string(nSize) + ".bmp";
	pSurface = SDL_LoadBMP(sBorders.c_str());
	if (!pSurface)
	{
		Destroy();
		return false;
	}
	pBorders = SDL_DisplayFormat(pSurface);
	SDL_FreeSurface(pSurface);

	return true;
}

CLevelSet::~CLevelSet()
{
	for (auto *lev : apLevels)
	{
		delete lev;
	}
	apLevels.clear();
}

void CLevelManager::SLevelSet::Reset()
{
	sName.clear();
	sAuthor.clear();
	sDate.clear();
	sURL.clear();
	sEmail.clear();
	sDifficulty.clear();
	sLevelFile.clear();
	sDescFile.clear();
}

void CLevelManager::AddLevelSet(
	std::string_view sName,
	std::string_view sAuthor,
	std::string_view sDate,
	std::string_view sURL,
	std::string_view sEmail,
	std::string_view sDifficulty,
	std::string_view sLevelFile,
	std::string_view sDescFile)
{
	m_aLevelSets.push_back({
		std::string(sName),
		std::string(sAuthor),
		std::string(sDate),
		std::string(sURL),
		std::string(sEmail),
		std::string(sDifficulty),
		std::string(sLevelFile),
		std::string(sDescFile)
	});
}

int CLevelManager::FindLevelSetDescriptorIndex(const char *szFilename) const
{
	for (size_t i = 0; i < m_aLevelSets.size(); ++i)
	{
		if (djStrCaseCmp(m_aLevelSets[i].sLevelFile, szFilename) == 0)
			return static_cast<int>(i);
	}
	return -1;
}

bool CLevelManager::FindLevelSetDescriptor(const char *szFilename, SLevelSet &LS) const
{
	for (const auto &item : m_aLevelSets)
	{
		if (djStrCaseCmp(item.sLevelFile, szFilename) == 0)
		{
			LS = item;
			return true;
		}
	}
	return false;
}

void CLevelManager::FindLevelSets(const std::string &sDirectory)
{
	std::vector<std::string> asFiles;
	djFindFiles(sDirectory, ".txt", asFiles);

	for (const auto &sLevelFile : asFiles)
	{
		std::string sName;
		std::string sAuthor;
		std::string sDate;
		std::string sURL;
		std::string sEMail;
		std::string sDifficulty;
		std::string sDescFile;

		if (sLevelFile.length() >= 4 && sLevelFile.substr(sLevelFile.length() - 4) == ".txt")
		{
			sDescFile = sLevelFile.substr(0, sLevelFile.length() - 4) + ".desc";
			std::string sDescPath = sDirectory + sDescFile;
			if (djFileExists(sDescPath))
			{
				std::ifstream desc(sDescPath);
				std::string line;
				while (std::getline(desc, line))
				{
					djStripCRLF(line);
					size_t pos = line.find('=');
					if (pos != std::string::npos)
					{
						std::string sField = line.substr(0, pos);
						std::string sValue = line.substr(pos + 1);
						if (djStrCaseCmp(sField, "name") == 0)            sName = sValue;
						else if (djStrCaseCmp(sField, "author") == 0)     sAuthor = sValue;
						else if (djStrCaseCmp(sField, "date") == 0)       sDate = sValue;
						else if (djStrCaseCmp(sField, "url") == 0)        sURL = sValue;
						else if (djStrCaseCmp(sField, "email") == 0)      sEMail = sValue;
						else if (djStrCaseCmp(sField, "difficulty") == 0) sDifficulty = sValue;
					}
				}
			}
			else
			{
				sDescFile.clear();
			}
		}

		if (sName.empty())
		{
			if (sLevelFile.length() >= 4 && sLevelFile.substr(sLevelFile.length() - 4) == ".txt")
				sName = sLevelFile.substr(0, sLevelFile.length() - 4);
			else
				sName = sLevelFile;
		}

		AddLevelSet(sName, sAuthor, sDate, sURL, sEMail, sDifficulty, sLevelFile, sDescFile);
	}
}
