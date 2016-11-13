//---------------------------------------------------------------------------
// djSettings.cpp
//
// Copyright (C) 2001-2007 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
#include "djSettings.h"
#include "djUtils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char g_szConfigFile[2048] = { 0 };

void CommitSettings()
{
	// Commit settings change
	g_Settings.SaveSettings(g_szConfigFile);
}

CdjSettings g_Settings;

//___________________________________________________________________________
//
// CdjSettings
//___________________________________________________________________________

CdjSettings::CdjSettings()
{
}

CdjSettings::~CdjSettings()
{
	ClearAllSettings();
}

bool CdjSettings::LoadSettings(const char *szFilename)
{
	char buf[2048];
	FILE *pIn = fopen(szFilename, "r");
	if (pIn==NULL)
		return false;
	fgets(buf, sizeof(buf), pIn);
	djStripCRLF(buf); // Strip newline char(s)
	while (!feof(pIn))
	{
		char *sz = strchr(buf, '=');
		if (sz)
		{
			*sz = 0;
			sz++;
			SdjSetting Setting;
			Setting.szKey   = new char[strlen(buf)+1];
			Setting.szValue = new char[strlen(sz)+1];
			strcpy(Setting.szKey,   buf);
			strcpy(Setting.szValue, sz);
			m_aSettings.push_back(Setting);
		}
		fgets(buf, sizeof(buf), pIn);
		djStripCRLF(buf); // Strip newline char(s)
	}
	fclose(pIn);
	return true;
}

bool CdjSettings::SaveSettings(const char *szFilename)
{
	FILE *pOut = fopen(szFilename, "w");
	if (pOut==NULL)
		return false;
	for ( int i=0; i<(int)m_aSettings.size(); i++ )
	{
		fprintf(pOut, "%s=%s\n", m_aSettings[i].szKey, m_aSettings[i].szValue);
	}
	fclose(pOut);
	return true;
}

void CdjSettings::SetSetting(const char *szKey, const char *szValue)
{
	// Try to find existing and change it if it exists
	for ( int i=0; i<(int)m_aSettings.size(); i++ )
	{
		if (!strcmp(szKey, m_aSettings[i].szKey))
		{
			delete[] m_aSettings[i].szValue;
			m_aSettings[i].szValue = new char[strlen(szValue)+1];
			strcpy(m_aSettings[i].szValue, szValue);
			return;
		}
	}
	// New setting
	SdjSetting Setting;
	Setting.szKey   = new char[strlen(szKey)+1];
	Setting.szValue = new char[strlen(szValue)+1];
	strcpy(Setting.szKey, szKey);
	strcpy(Setting.szValue, szValue);
	m_aSettings.push_back(Setting);
}

void CdjSettings::SetSettingBool(const char *szKey, bool bValue)
{
	char szBuf[128];
	sprintf(szBuf, "%d", bValue ? 1 : 0);
	SetSetting(szKey, szBuf);
}

void CdjSettings::SetSettingInt(const char *szKey, int nValue)
{
	char szBuf[128];
	sprintf(szBuf, "%d", nValue);
	SetSetting(szKey, szBuf);
}

void CdjSettings::SetSettingFloat(const char *szKey, float fValue)
{
	char szBuf[128];
	sprintf(szBuf, "%f", fValue);
	SetSetting(szKey, szBuf);
}

void CdjSettings::SetDefaultSetting(const char *szKey, const char *szValue)
{
	if (FindSetting(szKey))
		return;
	SetSetting(szKey, szValue);
}

char *CdjSettings::FindSetting(const char *szKey)
{
	for ( int i=0; i<(int)m_aSettings.size(); i++ )
	{
		if (!strcmp(szKey, m_aSettings[i].szKey))
			return m_aSettings[i].szValue;
	}
	return NULL;
}

void CdjSettings::ClearAllSettings()
{
	for ( int i=0; i<(int)m_aSettings.size(); i++ )
	{
		delete[] m_aSettings[i].szKey;
		delete[] m_aSettings[i].szValue;
	}
	m_aSettings.clear();
}
	
bool CdjSettings::GetSettingBool(const char *szKey, bool bDefaultValue)
{
	char *sz = FindSetting(szKey);
	if (sz==NULL || sz[0]==0)
		return bDefaultValue;
	if (sz[0]=='0' || sz[0]=='f' || sz[0]=='F' || sz[0]=='n' || sz[0]=='N')
		return false;
	return true;
}

int CdjSettings::GetSettingInt(const char *szKey, int nDefaultValue)
{
	char *sz = FindSetting(szKey);
	if (sz==NULL || sz[0]==0)
		return nDefaultValue;
	return atoi(sz);
}

float CdjSettings::GetSettingFloat(const char *szKey, float fDefaultValue)
{
	char *sz = FindSetting(szKey);
	if (sz==NULL || sz[0]==0)
		return fDefaultValue;
	return atof(sz);
}
