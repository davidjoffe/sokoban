//---------------------------------------------------------------------------
// djSettings.h
//
// Copyright (C) 2001-2026 David Joffe & Contributors
// https://grayskygames.com/
//---------------------------------------------------------------------------

#ifndef _DJSETTINGS_H_
#define _DJSETTINGS_H_

#include <string>
#include <string_view>
#include <vector>
#include <mutex>

class CdjSettings;

void CommitSettings();

extern std::string g_sConfigFile;
extern CdjSettings g_Settings;

class CdjSettings
{
public:
	CdjSettings() = default;
	~CdjSettings() = default;

	bool LoadSettings(const char *szFilename);
	bool SaveSettings(const char *szFilename) const;

	void SetSetting(std::string_view sKey, std::string_view sValue);
	void SetDefaultSetting(std::string_view sKey, std::string_view sValue);
	void SetDefaultSettingBool(std::string_view sKey, bool bValue)
	{
		SetDefaultSetting(sKey, bValue ? "1" : "0");
	}

	void SetSettingBool(std::string_view sKey, bool bValue);
	void SetSettingInt(std::string_view sKey, int nValue);
	void SetSettingFloat(std::string_view sKey, float fValue);

	const char *FindSetting(std::string_view sKey) const;
	void ClearAllSettings();

	bool GetSettingBool(std::string_view sKey, bool bDefaultValue = false) const;
	int GetSettingInt(std::string_view sKey, int nDefaultValue = 0) const;
	float GetSettingFloat(std::string_view sKey, float fDefaultValue = 0.0f) const;

protected:
	struct SdjSetting
	{
		std::string sKey;
		std::string sValue;
	};

	mutable std::mutex m_mutex;
	std::vector<SdjSetting> m_aSettings;
};

#endif
