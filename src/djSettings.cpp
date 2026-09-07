//---------------------------------------------------------------------------
// djSettings.cpp
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#include "djSettings.h"
#include "djUtils.h"
#include <fstream>
#include <sstream>

std::string g_sConfigFile;
CdjSettings g_Settings;

void CommitSettings()
{
	g_Settings.SaveSettings(g_sConfigFile.c_str());
}

bool CdjSettings::LoadSettings(const char *szFilename)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	std::ifstream in(szFilename);
	if (!in.is_open())
		return false;

	m_aSettings.clear();
	std::string line;
	while (std::getline(in, line))
	{
		djStripCRLF(line);
		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string val = line.substr(pos + 1);
			m_aSettings.push_back({std::move(key), std::move(val)});
		}
	}
	return true;
}

bool CdjSettings::SaveSettings(const char *szFilename) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	std::ofstream out(szFilename);
	if (!out.is_open())
		return false;

	for (const auto &item : m_aSettings)
	{
		out << item.sKey << '=' << item.sValue << '\n';
	}
	return true;
}

void CdjSettings::SetSetting(std::string_view sKey, std::string_view sValue)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto &item : m_aSettings)
	{
		if (item.sKey == sKey)
		{
			item.sValue = std::string(sValue);
			return;
		}
	}
	m_aSettings.push_back({std::string(sKey), std::string(sValue)});
}

void CdjSettings::SetDefaultSetting(std::string_view sKey, std::string_view sValue)
{
	if (FindSetting(sKey))
		return;
	SetSetting(sKey, sValue);
}

void CdjSettings::SetSettingBool(std::string_view sKey, bool bValue)
{
	SetSetting(sKey, bValue ? "1" : "0");
}

void CdjSettings::SetSettingInt(std::string_view sKey, int nValue)
{
	SetSetting(sKey, std::to_string(nValue));
}

void CdjSettings::SetSettingFloat(std::string_view sKey, float fValue)
{
	SetSetting(sKey, std::to_string(fValue));
}

const char *CdjSettings::FindSetting(std::string_view sKey) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	for (const auto &item : m_aSettings)
	{
		if (item.sKey == sKey)
			return item.sValue.c_str();
	}
	return nullptr;
}

void CdjSettings::ClearAllSettings()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_aSettings.clear();
}

bool CdjSettings::GetSettingBool(std::string_view sKey, bool bDefaultValue) const
{
	const char *val = FindSetting(sKey);
	if (!val || val[0] == '\0')
		return bDefaultValue;
	char c = val[0];
	return !(c == '0' || c == 'f' || c == 'F' || c == 'n' || c == 'N');
}

int CdjSettings::GetSettingInt(std::string_view sKey, int nDefaultValue) const
{
	const char *val = FindSetting(sKey);
	if (!val || val[0] == '\0')
		return nDefaultValue;
	return std::atoi(val);
}

float CdjSettings::GetSettingFloat(std::string_view sKey, float fDefaultValue) const
{
	const char *val = FindSetting(sKey);
	if (!val || val[0] == '\0')
		return fDefaultValue;
	return static_cast<float>(std::atof(val));
}
