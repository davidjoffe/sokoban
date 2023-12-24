//---------------------------------------------------------------------------
// djSettings.h
//
// Copyright (C) 2001-2023 David Joffe
// http://grayskygames.com/
//---------------------------------------------------------------------------
/*
\file   djSettings.h
\author David Joffe
\brief  Class that maintains config file for all included "apps"

The config file is simply a text file with a list of key=value mappings.
Both key and value are text strings.
*/
#ifndef _DJSETTINGS_H_
#define _DJSETTINGS_H_

#include <vector>
#include <string>

class CdjSettings;

extern void CommitSettings();

extern std::string g_sConfigFile;

//! Global settings
extern CdjSettings g_Settings;

//! Configuration
class CdjSettings
{
public:
	//! Constructor
	CdjSettings();
	//! Destructor
	~CdjSettings();
	
	//! Load all settings from given file. Return false if failed.
	bool LoadSettings(const char *szFilename);
	//! Save all settings to given file. Return false if failed.
	bool SaveSettings(const char *szFilename);
	
	//! Set the value of a specific setting. If the key currently does not exist, a new one is added.
	void SetSetting(const char *szKey, const char *szValue);
	//! Sets a key default value, i.e. sets the setting only if it doesn't exist already
	void SetDefaultSetting(const char *szKey, const char *szValue);
	//! Sets a key default value, i.e. sets the setting only if it doesn't exist already
	void SetDefaultSettingBool(const char *szKey, bool bValue) { SetDefaultSetting(szKey, bValue?"1":"0"); }



	//! Set the value of a specific setting. If the key currently does not exist, a new one is added.
	void SetSettingBool(const char *szKey, bool bValue);
	//! Set the value of a specific setting. If the key currently does not exist, a new one is added.
	void SetSettingInt(const char *szKey, int nValue);
	//! Set the value of a specific setting. If the key currently does not exist, a new one is added.
	void SetSettingFloat(const char *szKey, float fValue);


	
	//! Get a pointer to the value of a specific setting, or NULL if no such setting is defined
	char *FindSetting(const char *szKey);
	//! Clear all settings
	void ClearAllSettings();
	
	//! Get a boolean setting.
	bool GetSettingBool(const char *szKey, bool bDefaultValue=false);
	//! Get an integer setting.
	int GetSettingInt(const char *szKey, int nDefaultValue=0);
	//! Get a floating point setting.
	float GetSettingFloat(const char *szKey, float fDefaultValue=0);
	
protected:	
	// Internal structure used to store a single "setting".
	struct SdjSetting
	{
		char *szKey;
		char *szValue;
	};

	std::vector<SdjSetting> m_aSettings;
};

#endif
