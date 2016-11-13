//---------------------------------------------------------------------------
// Main.cpp
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------

#include "djDesktop.h"
#include "djSettings.h"
#include "GameRegistry.h"
#include "Games.h"
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif

bool g_bMustRedraw = true;

int HeartBeat();

//#define CONFIG_FILE ".syasokoban1.00"
#define CONFIG_FILE ".djgames"

int main(int, char**)
{
#ifndef WIN32 // <- This is not right, will be wrong on Linux, how to test for Mac?
	// If on Mac OS X, change into .app directory so we can find data files.
	chdir("SYASokoban.app");
	//chdir("FallingBlocks.app");
#endif

	RegisterGames();

	// Check if no games are registered
	if (g_GameRegistry.GetNumGameDescriptors()==0)
	{
		printf("No games registered\n");
		return -2;
	}

	/*
	SDL_Surface *pIcon = SDL_LoadBMP("data/sokoban/sokoban_icon.bmp");
	if (pIcon)
	{
		// FIXME: Crashes
		SDL_WM_SetIcon(pIcon, NULL);
	}
	*/

	memset(g_szConfigFile, 0, sizeof(g_szConfigFile));
	// Isn't there a better way to do this? e.g. system call. I think this messes up with su'd users.
	char *szHome = getenv("HOME");
	if (szHome)
	{
		strcpy(g_szConfigFile, szHome);
		strcat(g_szConfigFile, "/");
	}
	strcat(g_szConfigFile, CONFIG_FILE);
	
	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		char *szRet = SDL_GetError();
		printf("SDL Initialization failed (%s)\n", szRet);
		return -1;
	}

	// Load settings
	g_Settings.LoadSettings(g_szConfigFile);

	// Initialize desktop
	g_pScreen = new CdjDesktop;
	if (!g_pScreen->Initialize())
		return 0;

	// If only one game registered, skip to that immediately; otherwise,
	// show a menu allowing the user to select a game.
	if (g_GameRegistry.GetNumGameDescriptors()==1)
	{
		// Get the first (and only) game descriptor
		SGameDescriptor GD = g_GameRegistry.GetGameDescriptor(0);
		// Create the default app
		CdjItem *pGame = g_GameRegistry.AllocateGame(GD.szClassName);
		if (!pGame)
			return -1;
		SDL_WM_SetCaption(GD.szWindowTitle, NULL);

		// Expand to fill "desktop" area entirely
		pGame->SetExtents(0, 0, g_pScreen->m_nW, g_pScreen->m_nH);

		/*
		//pGame->SetExtents(20, 40, 800, 600);
		pGame->SetExtents(
			(g_pScreen->m_nW / 2) - (800 / 2),
			(g_pScreen->m_nH / 2) - (600 / 2),
			800,
			600);
		*/

		if (!pGame->Initialize())
			return -1;

		/*
		CdjWindow *pWindow = new CdjWindow;
		pWindow->Initialize();
		pWindow->SetName(pGame->GetName());
		pWindow->AddChild(pGame);
		g_pScreen->AddChild(pWindow);
		*/
		g_pScreen->AddChild(pGame);
	}
	else
	{
		// FIXME: TODO
	}

	// Enter main loop
	HeartBeat();

	// Clean up
	g_pScreen->DestroyAll();
	djDEL(g_pScreen);

	// Save setings
	g_Settings.SaveSettings(g_szConfigFile);

	//if (pIcon) SDL_FreeSurface(pIcon);

	// Shut down SDL
	SDL_Quit();

	UnregisterGames();
	
	return 0;
}

int HeartBeat()
{
	float fTime = (float)SDL_GetTicks() / 1000.0f;
	float fTimeOld = fTime;
	
	while (g_pScreen->m_bRunning)
	{
		fTime = (float)SDL_GetTicks() / 1000.0f;
		
		// Get mouse pos
		//SDL_GetMouseState(&g_pScreen->m_nMouseX, &g_pScreen->m_nMouseY);
		// Update events
		SDL_Event Event;
		SDLMod ModState;
		ModState = SDL_GetModState();
		while ( SDL_PollEvent(&Event) )
		{
			g_bMustRedraw = true; // Always redraw after an event
			g_pScreen->HandleEvent(Event, ModState);
		}
		
		// Update
		g_pScreen->UpdateAll(fTime - fTimeOld);

		if (g_bMustRedraw)
		{
			g_pScreen->DrawAll();

			// Draw mouse cursor
			if (g_pScreen->m_bShowCursor)
			{
				djBlit(
					g_pScreen->GetSurface(),
					g_pScreen->m_pCursor,
					g_pScreen->m_nMouseX - g_pScreen->m_nHotspotX,
					g_pScreen->m_nMouseY - g_pScreen->m_nHotspotY);
				//Debug:djDrawFilledRect(g_pScreen->GetSurface(), g_pScreen->m_nMouseX, g_pScreen->m_nMouseY, 1, 1, 0xFFFFFFFF);
			}
			
			SDL_Flip(g_pScreen->GetSurface());

			g_bMustRedraw = false;
		}
		else
		{
			// Sleep a little (we don't want to use SDL_WaitEvent though because there may
			// still be animated stuff so we also want to continuously 'Update' still).
			SDL_Delay(5);
		}
		
		fTimeOld = fTime;
	}
	
	return 0;
}
