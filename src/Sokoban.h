//---------------------------------------------------------------------------
// Sokoban.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _SOKOBAN_H_
#define _SOKOBAN_H_

#include "djItem.h"
#include <string>
#include <string_view>
#include <vector>

constexpr int LEVEL_MAX_W = 128;
constexpr int LEVEL_MAX_H = 128;

class djMessageBox;

namespace nsSokoban
{

class CChooseLevels;

class CLevel
{
public:
	CLevel() = default;
	~CLevel() = default;

	int x{-1};
	int y{-1};
	std::vector<std::string> apLines;
};

class CLevelSet
{
public:
	CLevelSet() = default;
	~CLevelSet();

	std::vector<CLevel*> apLevels;
	std::string m_sName;
};

class CLevelManager
{
public:
	CLevelManager() = default;
	~CLevelManager() = default;

	struct SLevelSet
	{
		void Reset();
		std::string sName;
		std::string sAuthor;
		std::string sDate;
		std::string sURL;
		std::string sEmail;
		std::string sDifficulty;
		std::string sLevelFile;
		std::string sDescFile;
	};

	size_t GetNumLevelSetDescriptors() const noexcept { return m_aLevelSets.size(); }
	const SLevelSet &GetLevelSetDescriptor(size_t uIndex) const { return m_aLevelSets[uIndex]; }
	bool FindLevelSetDescriptor(const char *szFilename, SLevelSet &LS) const;
	int FindLevelSetDescriptorIndex(const char *szFilename) const;

	void AddLevelSet(
		std::string_view sName,
		std::string_view sAuthor,
		std::string_view sDate,
		std::string_view sURL,
		std::string_view sEmail,
		std::string_view sDifficulty,
		std::string_view sLevelFile,
		std::string_view sDescFile);

	void FindLevelSets(const std::string &sDirectory);

protected:
	std::vector<SLevelSet> m_aLevelSets;
};

class CSokoban : public CdjItem
{
public:
	CSokoban();
	~CSokoban() override;

	bool Initialize() override;
	bool Destroy() override;
	bool OnEvent(SDL_Event &Event, SDL_Keymod ModState) override;
	bool Update(float dt) override;
	void Draw(SDL_Renderer *pRenderer, int nX, int nY) override;
	bool OnMouseButtonDown(int nButton) override;
	bool OnMouseMove(int nX, int nY, int nDX, int nDY) override;

protected:
	bool IsLevelCompleted(int nLevelNum);
	int  GetNumMovesTaken(int nLevelNum);
	void AddCompletedLevel(int nLevelNum);
	void TryConvertVersion1Settings();

	std::string GetLevelsCompletedSettingsKey();

	void UndoMove();
	void TryMove(int dx, int dy);
	void MoveHero(int dx, int dy);
	void SetHeroDirection(int dx, int dy);
	bool LoadLevelset(const char *szFilename);
	int SetLevel(int iLev);

	bool HaveLevel() const noexcept { return (m_pCurSet && m_iCurLev >= 0); }
	int GetNumLevels() const noexcept { return m_pCurSet ? static_cast<int>(m_pCurSet->apLevels.size()) : 0; }

	int m_nLevelW{-1};
	int m_nLevelH{-1};
	int m_nHeroX{-1};
	int m_nHeroY{-1};
	bool m_bWon{false};
	bool m_bGotMouseClick{false};
	int m_iCurLev{-1};
	int m_iNumMoves{0};
	int m_iNumPushes{0};
	char m_CurLevel[LEVEL_MAX_W][LEVEL_MAX_H]{};

	int m_nMouseX{0};
	int m_nMouseY{0};
	bool m_bShadows{true};
	int m_nHeroDirection{0};
	int m_nMovesTaken{-1};

	struct STileSet
	{
		STileSet() = default;
		~STileSet() { Destroy(); }
		void Destroy();
		bool Load(SDL_Renderer *pRenderer, const char *szDirectory, int nSize);

		SDL_Texture *pSprites{nullptr};
		SDL_Texture *pBorders{nullptr};
		int nW{0};
		int nH{0};
	};

	std::vector<STileSet*> m_apTileSets;
	STileSet *m_pTileSet{nullptr};

	void LoadTileSets(const char *szDirectory);
	void ClearTileSets();
	void ChooseTileSet();

	SDL_Texture *m_pSokobanHeader{nullptr};
	SDL_Texture *m_pAbout{nullptr};
	float m_fPushedTimer{0.0f};
	bool m_bShowingAbout{false};

	void SetShowingAbout(bool b);

	CChooseLevels *m_pChooseLevels{nullptr};
	void CloseChooseLevels();

	djMessageBox *m_pCloseConfirm{nullptr};
	void ShowCloseConfirm();
	void HideCloseConfirm();

	enum
	{
		sokBLANK        = 1,
		sokCRATE        = 39,
		sokCRATEDEST    = 9,
		sokCRATEDONE    = 47,
		sokWALL         = 16,
		sokWALL_LAST    = sokWALL + 15,
		sokWALL_CRACKED = sokWALL + 5 + 16
	};

	constexpr bool IsWall(char c) const noexcept { return (c >= sokWALL && c <= sokWALL_LAST) || c == sokWALL_CRACKED; }
	constexpr bool IsCrate(char c) const noexcept { return (c == sokCRATE || c == sokCRATEDONE); }
	constexpr bool CastsShadow(char c) const noexcept { return IsWall(c) || IsCrate(c); }

	int m_nTargetX{-1};
	int m_nTargetY{-1};
	int m_FindMap[LEVEL_MAX_W][LEVEL_MAX_H]{};
	void FindTarget(int nX, int nY, int nPathLen);

	struct SAutoMove
	{
		int dx;
		int dy;
	};
	std::vector<SAutoMove> m_aAutoPath;
	void ClearAutoPath();
	bool m_bBusyFollowingAutoPath{false};
	float m_fAutoPathFollowTimer{0.0f};

	bool m_bMayPickCrates{true};
	int m_nCrateX{-1};
	int m_nCrateY{-1};

	CLevelManager *m_pLevelManager{nullptr};
	CLevelSet *m_pCurSet{nullptr};
	std::string m_sCurSet;
	CLevelManager::SLevelSet m_LevelSet;

	struct SUndoMove
	{
		bool bCrate;
		int x, y;
		int cxB, cyB, cxA, cyA;
		int nHeroDirection;
	};
	std::vector<SUndoMove> m_aUndo;
};

}

#endif
