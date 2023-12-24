//---------------------------------------------------------------------------
// GameRegistry.cpp
//
// Copyright (C) 2002-2023 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------

#include "GameRegistry.h"
#include "djUtils.h"

CGameRegistry g_GameRegistry;

CGameRegistry::CGameRegistry()
{
}

CGameRegistry::~CGameRegistry()
{
	for ( int i=0; i<(int)m_aDescriptors.size(); i++ )
	{
		djDELV(m_aDescriptors[i].szClassName);
		djDELV(m_aDescriptors[i].szName);
		djDELV(m_aDescriptors[i].szVersion);
		djDELV(m_aDescriptors[i].szWindowTitle);
	}
	m_aDescriptors.clear();
}

bool CGameRegistry::RegisterGame(const char *szClassName, const char *szName, const char *szVersion, const char *szWindowTitle, PROC_ALLOCATEGAME pProcAllocate)
{
	SGameDescriptor GD;
	GD.szClassName = djStrDup(szClassName);
	GD.szName = djStrDup(szName);
	GD.szVersion = djStrDup(szVersion);
	GD.szWindowTitle = djStrDup(szWindowTitle);
	GD.pProcAllocate = pProcAllocate;
	m_aDescriptors.push_back(GD);
	return true;
}

void CGameRegistry::UnregisterGame(const char *szClassName)
{
	for ( int i=0; i<(int)m_aDescriptors.size(); i++ )
	{
		if (!strcmp(m_aDescriptors[i].szClassName, szClassName))
		{
			djDELV(m_aDescriptors[i].szClassName);
			djDELV(m_aDescriptors[i].szName);
			djDELV(m_aDescriptors[i].szVersion);
			djDELV(m_aDescriptors[i].szWindowTitle);
			return;
		}
	}
	djASSERT(0); // Not found
}

CdjItem *CGameRegistry::AllocateGameByIndex(UINT uIndex)
{
	return (m_aDescriptors[uIndex].pProcAllocate)();
}

CdjItem *CGameRegistry::AllocateGame(const char *szClassName)
{
	for ( UINT i=0; i<m_aDescriptors.size(); i++ )
	{
		if (!strcmp(m_aDescriptors[i].szClassName, szClassName))
			return (m_aDescriptors[i].pProcAllocate)();
	}
	return NULL;
}
