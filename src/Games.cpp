//---------------------------------------------------------------------------
// Games.cpp
//
// Copyright (C) 2002-2024 David Joffe
// https://grayskygames.com/
//---------------------------------------------------------------------------

// Register games in the game registry here.

// To exclude games from the build, just comment out their
// registration entry here.

// See GameRegistry.h for more information.

#include "GameRegistry.h"
#include "Sokoban.h"
//#include "Tetris.h"
//#include "Bao.h"
#include "Games.h"

CdjItem *AllocateSokoban() { return new nsSokoban::CSokoban; }
//CdjItem *AllocateTetris() { return new djTetris::CTetris; }
//CdjItem *AllocateBao() { return new djBao::CBao; }

void RegisterGames()
{
	g_GameRegistry.RegisterGame("CSokoban", "Sokoban", "2.01", "Sokoban", AllocateSokoban);
	//g_GameRegistry.RegisterGame("CTetris", "Falling Blocks Game", "1.00", "Falling Blocks Game", AllocateTetris);
	//g_GameRegistry.RegisterGame("CBao", "Bao", "1.00", "Bao", AllocateBao);
}

void UnregisterGames()
{
	//g_GameRegistry.UnregisterGame("CBao");
	//g_GameRegistry.UnregisterGame("CTetris");
	g_GameRegistry.UnregisterGame("CSokoban");
}
