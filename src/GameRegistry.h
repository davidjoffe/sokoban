//---------------------------------------------------------------------------
// GameRegistry.h
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _GAMEREGISTRY_H_
#define _GAMEREGISTRY_H_

#include "djItem.h"
#include <string>
#include <string_view>
#include <vector>

typedef CdjItem *(*PROC_ALLOCATEGAME)();

struct SGameDescriptor
{
	std::string sClassName;
	std::string sName;
	std::string sVersion;
	std::string sWindowTitle;
	PROC_ALLOCATEGAME pProcAllocate{nullptr};
};

class CGameRegistry
{
public:
	CGameRegistry() = default;
	virtual ~CGameRegistry() = default;

	bool RegisterGame(
		std::string_view sClassName,
		std::string_view sName,
		std::string_view sVersion,
		std::string_view sWindowTitle,
		PROC_ALLOCATEGAME pProcAllocate);

	void UnregisterGame(std::string_view sClassName);

	size_t GetNumGameDescriptors() const noexcept { return m_aDescriptors.size(); }
	const SGameDescriptor &GetGameDescriptor(size_t uIndex) const { return m_aDescriptors[uIndex]; }

	CdjItem *AllocateGameByIndex(size_t uIndex);
	CdjItem *AllocateGame(std::string_view sClassName);

protected:
	std::vector<SGameDescriptor> m_aDescriptors;
};

extern CGameRegistry g_GameRegistry;

#define REGISTER_GAME(szClassName, szName, szVersion, szWindowTitle) \
	CdjItem *ALLOCATE##szClassName() { return new szClassName; } \
	static bool UNIQUECLASS##szClassName = g_GameRegistry.RegisterGame(#szClassName, szName, szVersion, szWindowTitle, ALLOCATE##szClassName);

#endif
