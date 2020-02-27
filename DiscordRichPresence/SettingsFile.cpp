#include "stdafx.h"
#include "SettingsFile.h"

PluginSettings g_pluginSettings;

std::string GetExecutableFileName()
{
	char buffer[MAX_PATH];
	(void)GetModuleFileNameA(NULL, buffer, MAX_PATH); // Result is ignored; empty string returned on failure

	return std::string(buffer);
}

//used to convert strings to a usable string in MessageBox, example:
//std::wstring stemp = s2ws(executablePath);
//MessageBox(0, stemp.c_str(), L"", MB_OK);
//https://stackoverflow.com/a/27296
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::string GetExecutablePath()
{
	std::string executableFileName = GetExecutableFileName();
	if (executableFileName.length() > 0)
	{
		return executableFileName.substr(0, executableFileName.find_last_of("\\/"));
	}

	return "";
}

std::string GetSettingsFilePath()
{
	std::string executablePath = GetExecutablePath();
	if (executablePath.size() == 0)
		return ""; // Couldn't load

	std::string settingsFilePath = executablePath;
	settingsFilePath.append("\\Plugins\\DiscordRichPresence\\settings.ini");
	return settingsFilePath;
}

void SaveSettingsFile()
{
	std::string settingsFilePath = GetSettingsFilePath();
	std::ofstream settingsFileWrite(settingsFilePath);
	settingsFileWrite << "# Discord Rich Presence configuration" << "\n";
	settingsFileWrite << "#####################################" << "\n";

	if (g_pluginSettings.DisplayTitleInStatus)
		settingsFileWrite << "DisplayTitleInStatus:true" << "\n";
	else
		settingsFileWrite << "DisplayTitleInStatus:false" << "\n";

	if (g_pluginSettings.ShowElapsedTime)
		settingsFileWrite << "ShowElapsedTime:true" << "\n";
	else
		settingsFileWrite << "ShowElapsedTime:false" << "\n";

	settingsFileWrite << "ApplicationID:" << g_pluginSettings.ApplicationID << "\n";

	if (g_pluginSettings.SendWhenPausedOrStopped)
		settingsFileWrite << "SendWhenPausedOrStopped:true" << "\n";
	else
		settingsFileWrite << "SendWhenPausedOrStopped:false" << "\n";

	settingsFileWrite.close();
}


bool GetBooleanSettingsFileValue(std::string const& line, char const* label)
{
	std::string value = line.substr(strlen(label));
	if (value == "true")
		return true;
	else if (value == "false")
		return false;

	return false;
}

void LoadSettingsFile()
{
	// Set some defaults
	g_pluginSettings.DisplayTitleInStatus = false;
	g_pluginSettings.ApplicationID = "0";

	std::string settingsFilePath = GetSettingsFilePath();

	// Attempt to load settings file.
	std::ifstream settingsFileRead(settingsFilePath);
	if (settingsFileRead.is_open())
	{
		while (settingsFileRead.good())
		{
			std::string line;
			std::getline(settingsFileRead, line);
			if (line.length() == 0)
				continue;

			if (line[0] == '#') // Comments
				continue;

			static const char* displayTileInStatus_label = "DisplayTitleInStatus:";
			static const char* showElapsedTime_label = "ShowElapsedTime:";
			static const char* applicationID_label = "ApplicationID:";
			static const char* sendWhenPausedOrStopped_label = "SendWhenPausedOrStopped:";

			if (line.find(displayTileInStatus_label) == 0)
			{
				g_pluginSettings.DisplayTitleInStatus = GetBooleanSettingsFileValue(line, displayTileInStatus_label);
			}
			else if (line.find(showElapsedTime_label) == 0)
			{
				g_pluginSettings.ShowElapsedTime = GetBooleanSettingsFileValue(line, showElapsedTime_label);
			}
			else if (line.find(applicationID_label) == 0)
			{
				std::string value = line.substr(strlen(applicationID_label));
				g_pluginSettings.ApplicationID = value;
			}
			else if (line.find(sendWhenPausedOrStopped_label) == 0)
			{
				g_pluginSettings.SendWhenPausedOrStopped = GetBooleanSettingsFileValue(line, sendWhenPausedOrStopped_label);
			}
		}
	}
	else
	{
		// No settings file was found. Create one.
		SaveSettingsFile();
	}
}