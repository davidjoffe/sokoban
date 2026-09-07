//---------------------------------------------------------------------------
// GameRegistry.cpp
//
// Copyright (C) 2002-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "GameRegistry.h"
#include "djUtils.h"

CGameRegistry g_GameRegistry;

bool CGameRegistry::RegisterGame(
	std::string_view sClassName,
	std::string_view sName,
	std::string_view sVersion,
	std::string_view sWindowTitle,
	PROC_ALLOCATEGAME pProcAllocate)
{
	m_aDescriptors.push_back({
		std::string(sClassName),
		std::string(sName),
		std::string(sVersion),
		std::string(sWindowTitle),
		pProcAllocate
	});
	return true;
}

void CGameRegistry::UnregisterGame(std::string_view sClassName)
{
	std::erase_if(m_aDescriptors, [sClassName](const SGameDescriptor &gd) {
		return gd.sClassName == sClassName;
	});
}

CdjItem *CGameRegistry::AllocateGameByIndex(size_t uIndex)
{
	if (uIndex < m_aDescriptors.size() && m_aDescriptors[uIndex].pProcAllocate)
		return (m_aDescriptors[uIndex].pProcAllocate)();
	return nullptr;
}

CdjItem *CGameRegistry::AllocateGame(std::string_view sClassName)
{
	for (const auto &gd : m_aDescriptors)
	{
		if (gd.sClassName == sClassName && gd.pProcAllocate)
			return (gd.pProcAllocate)();
	}
	return nullptr;
}
