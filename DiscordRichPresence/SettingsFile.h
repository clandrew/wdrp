#pragma once

struct PluginSettings
{
	bool DisplayTitleInStatus;
	bool ShowElapsedTime;
	std::string ApplicationID;
	bool SendWhenPausedOrStopped;
};

std::string GetSettingsFilePath();
void SaveSettingsFile();
void LoadSettingsFile();

extern PluginSettings g_pluginSettings;