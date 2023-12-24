//---------------------------------------------------------------------------
// GameRegistry.h
//
// Copyright (C) 2002-2023 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
#ifndef _GAMEREGISTRY_H_
#define _GAMEREGISTRY_H_

#include "djItem.h"

//! Game allocate function
typedef CdjItem *(*PROC_ALLOCATEGAME)();

/*!\brief Game registry descriptor for a single game.
\author   David Joffe
*/
struct SGameDescriptor
{
	char *szClassName;		//!< C++ class name, e.g. "CSokoban"
	char *szName;			//!< Human-friendly game name, e.g. "Sokoban"
	char *szVersion;		//!< Current game version number, e.g. "1.02"
	char *szWindowTitle;	//!< Main app/window title to use, e.g. "Sokoban"

	PROC_ALLOCATEGAME pProcAllocate; //!< Widget allocate procedure.
};


/*!\brief Game registry.
\author   David Joffe

All possible games are registered in the game registry, e.g.
Sokoban, Tetris, whatever. A game is essentially just a widget,
but includes some higher level info about the game, i.e. name,
version, author, information required to put game in menu etc,
game directory etc. The game registry simply maintains an array
of \ref SGameDescriptor structs, each one containing descriptor
information for a registered game type.

The main app will start with a menu of registered games, allowing
the user to select what game to play. If only one game registered,
the main menu phase is simply skipped, and jumps right into that
game. This allows the framework to be more readily modified for
a single game, simply delete the baggage associated with any other
games.

The game registry acts as a sort of object factory; it can be
used to generically allocate instances of specific game widgets.

*/

class CGameRegistry
{
public:
	///----- \name Constructors/destructors
	//@{

	//! Constructor
	CGameRegistry();
	//! Destructor
	virtual ~CGameRegistry();

	//@}
	///----- \name Registering and unregistering game types
	//@{

	//! Register a game type. See \ref SGameDescriptor for explanations of all the parameters.
	bool RegisterGame(const char *szClassName, const char *szName, const char *szVersion, const char *szWindowTitle, PROC_ALLOCATEGAME pProcAllocate);
	//! Unregister a game type.
	void UnregisterGame(const char *szClassName);

	//@}
	///----- \name Registry information
	//@{

	//! Get number of registered game types
	UINT GetNumGameDescriptors() const { return m_aDescriptors.size(); }
	//! Get pointer to game descriptor (by index)
	SGameDescriptor GetGameDescriptor(UINT uIndex) { return m_aDescriptors[uIndex]; }

	//@}
	///----- \name Allocating game instances
	//@{
	
	//! Allocate a game by descriptor index.
	CdjItem *AllocateGameByIndex(UINT uIndex);
	//! Allocate a game by C++ class name. Case-sensitive.
	CdjItem *AllocateGame(const char *szClassName);

	//@}
protected:
	///----- \name Protected members
	//@{

	//! Array of game descriptors.
	std::vector<SGameDescriptor> m_aDescriptors;

	//@}
};

//! The game registry.
extern CGameRegistry g_GameRegistry;

//! Game register helper macro
#define REGISTER_GAME(szClassName, szName, szVersion, szWindowTitle) \
	CdjItem *ALLOCATE##szClassName() { return new szClassName; } \
	static bool UNIQUECLASS##szClassName = g_GameRegistry.RegisterGame("szClassName", szName, szVersion, szWindowTitle, ALLOCATE##szClassName);

#endif
