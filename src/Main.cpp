//---------------------------------------------------------------------------
// Main.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djDesktop.h"
#include "djSettings.h"
#include "GameRegistry.h"
#include "Games.h"
#include <cstdlib>
#include <atomic>
#include <filesystem>
#include <iostream>

#ifndef WIN32
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

std::atomic<bool> g_bMustRedraw{true};

int HeartBeat();

constexpr const char* CONFIG_FILE = ".djgames";

int main(int, char**)
{
#ifndef WIN32
	if (std::filesystem::exists("SYASokoban.app"))
	{
		std::filesystem::current_path("SYASokoban.app");
	}
#endif

	RegisterGames();

	if (g_GameRegistry.GetNumGameDescriptors() == 0)
	{
		std::cerr << "No games registered\n";
		return -2;
	}

	std::string s;
#ifdef WIN32
	const char *appData = std::getenv("APPDATA");
	if (appData)
	{
		s = appData;
		djAppendPathS(s, "GraySkyGames");
		djEnsureFolderTreeExists(s);
		if (!djFolderExists(s))
			s = CONFIG_FILE;
		else
			djAppendPathS(s, CONFIG_FILE);
	}
	else
	{
		s = CONFIG_FILE;
	}
#else
	const char *home = std::getenv("HOME");
	s = home ? home : ".";
	djAppendPathS(s, CONFIG_FILE);
#endif
	g_sConfigFile = s;

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL3 Initialization failed: " << SDL_GetError() << '\n';
		return -1;
	}

	g_Settings.LoadSettings(g_sConfigFile.c_str());

	g_pScreen = new CdjDesktop;
	if (!g_pScreen->Initialize())
	{
		delete g_pScreen;
		g_pScreen = nullptr;
		SDL_Quit();
		return 0;
	}

	if (g_GameRegistry.GetNumGameDescriptors() == 1)
	{
		const auto &GD = g_GameRegistry.GetGameDescriptor(0);
		CdjItem *pGame = g_GameRegistry.AllocateGame(GD.sClassName);
		if (!pGame)
		{
			delete g_pScreen;
			g_pScreen = nullptr;
			SDL_Quit();
			return -1;
		}

		SDL_SetWindowTitle(g_pScreen->GetWindow(), GD.sWindowTitle.c_str());
		pGame->SetExtents(0, 0, g_pScreen->m_nW, g_pScreen->m_nH);

		if (!pGame->Initialize())
		{
			delete pGame;
			delete g_pScreen;
			g_pScreen = nullptr;
			SDL_Quit();
			return -1;
		}
		g_pScreen->AddChild(pGame);
	}

	HeartBeat();

	g_pScreen->DestroyAll();
	delete g_pScreen;
	g_pScreen = nullptr;

	g_Settings.SaveSettings(g_sConfigFile.c_str());
	SDL_Quit();
	UnregisterGames();

	return 0;
}

#ifdef WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
	return main(__argc, __argv);
}
#endif

int HeartBeat()
{
	float fTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
	float fTimeOld = fTime;

	while (g_pScreen->m_bRunning.load(std::memory_order_relaxed))
	{
		fTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;

		SDL_Event Event;
		SDL_Keymod ModState = SDL_GetModState();
		while (SDL_PollEvent(&Event))
		{
			SDL_ConvertEventToRenderCoordinates(g_pScreen->GetRenderer(), &Event);
			g_bMustRedraw.store(true, std::memory_order_relaxed);
			g_pScreen->HandleEvent(Event, ModState);
		}

		g_pScreen->UpdateAll(fTime - fTimeOld);

		if (g_bMustRedraw.load(std::memory_order_relaxed))
		{
			g_pScreen->DrawAll();

			if (g_pScreen->m_bShowCursor && g_pScreen->m_pCursor)
			{
				djBlit(
					g_pScreen->GetRenderer(),
					g_pScreen->m_pCursor,
					g_pScreen->m_nMouseX - g_pScreen->m_nHotspotX,
					g_pScreen->m_nMouseY - g_pScreen->m_nHotspotY);
			}

			SDL_RenderPresent(g_pScreen->GetRenderer());
			g_bMustRedraw.store(false, std::memory_order_relaxed);
		}
		else
		{
			SDL_Delay(5);
		}

		fTimeOld = fTime;
	}

	return 0;
}
