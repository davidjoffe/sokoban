//---------------------------------------------------------------------------
// Games.cpp
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "GameRegistry.h"
#include "Sokoban.h"
#include "Games.h"

CdjItem *AllocateSokoban() { return new nsSokoban::CSokoban; }

void RegisterGames()
{
	g_GameRegistry.RegisterGame("CSokoban", "Sokoban", "2.0.3", "Still Yet Another Sokoban", AllocateSokoban);
}

void UnregisterGames()
{
	g_GameRegistry.UnregisterGame("CSokoban");
}
