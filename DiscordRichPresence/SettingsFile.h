#pragma once

struct PluginSettings
{
	bool DisplayTitleInStatus;
	bool ShowElapsedTime;
	std::string ApplicationID;
};

std::string GetSettingsFilePath();
void SaveSettingsFile();
void LoadSettingsFile();

extern PluginSettings g_pluginSettings;