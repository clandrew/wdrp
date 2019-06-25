// DiscordRichPresence.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Resource.h"

// Forward declares
typedef void (DISCORD_EXPORT *Discord_InitializeFn)(const char*, DiscordEventHandlers*, int, const char*);
typedef void (DISCORD_EXPORT *Discord_ShutdownFn)(void);
typedef void (DISCORD_EXPORT *Discord_UpdatePresenceFn)(const DiscordRichPresence*);
typedef void (DISCORD_EXPORT *Discord_RunCallbacksFn)(void);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ReportIdleStatus();

// plugin version (don't touch this)
#define GPPHDR_VER 0x10

// plugin name/title (change this to something you like)
char PLUGIN_NAME[] = "Discord Rich Presence";

// Variables
HMODULE hDiscordModule{};
Discord_InitializeFn initializeFn{};
Discord_ShutdownFn shutdownFn{};
Discord_UpdatePresenceFn updatePresenceFn{};
Discord_RunCallbacksFn runCallbacksFn{};
WNDPROC lpWndProcOld = 0;

enum PlaybackState // Enumeration values set up to match IPC_ISPLAYING
{
	Stopped = 0,
	Playing = 1,
	Paused = 3
};
class PresenceInfo
{
	DiscordRichPresence m_presence;
	std::string m_stateBuffer;
	std::string m_detailsBuffer;

public:
	PresenceInfo()
	{
		memset(&m_presence, 0, sizeof(m_presence));
		m_presence.largeImageKey = "winamp-logo";
		m_presence.instance = 1;
		CurrentPlaybackState = Stopped;
	}
	PlaybackState CurrentPlaybackState;

	void SetStateText(char const* str)
	{
		m_stateBuffer = str;
		m_presence.state = m_stateBuffer.c_str();
	}

	void SetDetails(char const* str)
	{
		m_detailsBuffer = str;
		m_presence.details = m_detailsBuffer.c_str();
	}

	void ClearDetails()
	{
		m_detailsBuffer.clear();
		m_presence.details = nullptr;
	}

	void SetStartTimestamp(__int64 timestamp)
	{
		m_presence.startTimestamp = timestamp;
	}

	void PostToDiscord()
	{
		updatePresenceFn(&m_presence);
		runCallbacksFn();
	}

};
std::unique_ptr<PresenceInfo> lastPresence;

struct PluginSettings
{
    bool DisplayTitleInSettings;
    std::string ApplicationID;
} pluginSettings;

// main structure with plugin information, version, name...
typedef struct {
    int version;                   // version of the plugin structure
    char *description;             // name/title of the plugin 
    int(*init)();                 // function which will be executed on init event
    void(*config)();              // function which will be executed on config event
    void(*quit)();                // function which will be executed on quit event
    HWND hwndParent;               // hwnd of the Winamp client main window (stored by Winamp when dll is loaded)
    HINSTANCE hDllInstance;        // hinstance of this plugin DLL. (stored by Winamp when dll is loaded) 
} winampGeneralPurposePlugin;

// these are callback functions/events which will be called by Winamp
int  init(void);
void config(void);
void quit(void);

// this structure contains plugin information, version, name...
// GPPHDR_VER is the version of the winampGeneralPurposePlugin (GPP) structure
winampGeneralPurposePlugin plugin = {
    GPPHDR_VER,  // version of the plugin, defined in "gen_myplugin.h"
    PLUGIN_NAME, // name/title of the plugin, defined in "gen_myplugin.h"
    init,        // function name which will be executed on init event
    config,      // function name which will be executed on config event
    quit,        // function name which will be executed on quit event
    0,           // handle to Winamp main window, loaded by winamp when this dll is loaded
    0            // hinstance to this dll, loaded by winamp when this dll is loaded
}; 

static void handleDiscordReady(const DiscordUser* connectedUser)
{
}

static void handleDiscordError(int errcode, const char* message)
{
}

static void handleDiscordDisconnected(int errcode, const char* message)
{
}

static void handleDiscordJoinGame(const char* secret)
{
}

static void handleDiscordSpectateGame(const char* secret)
{
}

static void handleDiscordJoinRequest(const DiscordUser* request)
{
}

void InitDiscord()
{
    if (!hDiscordModule)
        return;

    if (pluginSettings.ApplicationID == "0")
        return;

    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handleDiscordReady;
    handlers.errored = handleDiscordError;
    handlers.disconnected = handleDiscordDisconnected;
    handlers.joinGame = handleDiscordJoinGame;
    handlers.spectateGame = handleDiscordSpectateGame;
    handlers.joinRequest = handleDiscordJoinRequest;

    std::string applicationID = pluginSettings.ApplicationID.c_str();
    int autoRegister = 1;
    const char* noSteamID = nullptr;
    initializeFn(applicationID.c_str(), &handlers, autoRegister, noSteamID);
    runCallbacksFn();
}

std::string GetExecutableFileName()
{
    char buffer[MAX_PATH];
    (void)GetModuleFileNameA(NULL, buffer, MAX_PATH); // Result is ignored; empty string returned on failure

    return std::string(buffer);
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

    if (pluginSettings.DisplayTitleInSettings)
        settingsFileWrite << "DisplayTitleInStatus:true" << "\n";
    else
        settingsFileWrite << "DisplayTitleInStatus:false" << "\n";

    settingsFileWrite << "ApplicationID:" << pluginSettings.ApplicationID << "\n";
    settingsFileWrite.close();
}

void LoadSettingsFile()
{
    // Set some defaults
    pluginSettings.DisplayTitleInSettings = false;
    pluginSettings.ApplicationID = "0";

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
            static const char* applicationID_label = "ApplicationID:";

            if (line.find(displayTileInStatus_label) == 0)
            {
                std::string value = line.substr(strlen(displayTileInStatus_label));
                if (value == "true")
                    pluginSettings.DisplayTitleInSettings = true;
                else if (value == "false")
                    pluginSettings.DisplayTitleInSettings = false;
            }
            else if (line.find(applicationID_label) == 0)
            {
                std::string value = line.substr(strlen(applicationID_label));
                pluginSettings.ApplicationID = value;
            }
        }
    }
    else
    {
        // No settings file was found. Create one.
        SaveSettingsFile();
    }
}

void LoadDiscordEntrypoints()
{
    if (!hDiscordModule)
        return;

    initializeFn = (Discord_InitializeFn)GetProcAddress(hDiscordModule, "Discord_Initialize");
    shutdownFn = (Discord_ShutdownFn)GetProcAddress(hDiscordModule, "Discord_Shutdown");
    updatePresenceFn = (Discord_UpdatePresenceFn)GetProcAddress(hDiscordModule, "Discord_UpdatePresence");
    runCallbacksFn = (Discord_RunCallbacksFn)GetProcAddress(hDiscordModule, "Discord_RunCallbacks");
}

void CALLBACK TimerProc(HWND Arg1, UINT Arg2, UINT_PTR Arg3, DWORD Arg4)
{
	assert(lastPresence);
	assert(lastPresence->CurrentPlaybackState == Playing);

	int currentPlaybackPositionInMSMode = 0;
	int playbackPosition = SendMessage(plugin.hwndParent, WM_WA_IPC, currentPlaybackPositionInMSMode, IPC_GETOUTPUTTIME);
	int playbackPositionInSeconds = playbackPosition / 1000;

	std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
	long long dtn = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

	lastPresence->SetStartTimestamp(dtn - playbackPositionInSeconds);

	lastPresence->PostToDiscord();
}

int init() 
{
    if (IsWindowUnicode(plugin.hwndParent))
        lpWndProcOld = (WNDPROC)SetWindowLongW(plugin.hwndParent, GWL_WNDPROC, (LONG)WndProc);
    else
        lpWndProcOld = (WNDPROC)SetWindowLongA(plugin.hwndParent, GWL_WNDPROC, (LONG)WndProc);

    LoadSettingsFile();

    hDiscordModule = LoadLibrary(L"Plugins\\DiscordRichPresence\\discord-rpc.dll");
    LoadDiscordEntrypoints();

    InitDiscord();

	lastPresence.reset(new PresenceInfo());

    ReportIdleStatus();

    return 0;
}

void ShutdownDiscord()
{
    if (hDiscordModule)
    {
        shutdownFn();
    }
}


void ReportIdleStatus()
{
    if (!hDiscordModule)
        return;

    if (pluginSettings.ApplicationID == "0")
        return;

	lastPresence->CurrentPlaybackState = Stopped;
	lastPresence->SetStartTimestamp(0);
	lastPresence->SetStateText("(Idle)");
	lastPresence->ClearDetails();

	lastPresence->PostToDiscord();
}

void ReportCurrentSongStatus(PlaybackState playbackState)
{
    if (!hDiscordModule)
        return;

    if (pluginSettings.ApplicationID == "0")
        return;
	
	assert(playbackState != Stopped);
	lastPresence->CurrentPlaybackState = playbackState;
	lastPresence->SetStartTimestamp(0);
	lastPresence->SetStateText(playbackState == Playing ? "(Playing)" : "(Paused)");
	
    std::string detailsMessage;
    if (pluginSettings.DisplayTitleInSettings)
    {
		std::wstring title = (wchar_t *)SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_PLAYING_TITLE);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convertWideToUTF8;
		detailsMessage = convertWideToUTF8.to_bytes(title);
    }
    else
    {
        detailsMessage = "";
    }
	lastPresence->SetDetails(detailsMessage.c_str());
	lastPresence->PostToDiscord();
}

void UpdateRichPresenceDetails()
{
	LONG isPlayingResult = SendMessage(plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

	if (isPlayingResult == Playing)
	{
		// Playing
		SetTimer(plugin.hwndParent, 1, 1000, TimerProc);
		ReportCurrentSongStatus(Playing);
	}
	else if (isPlayingResult == Paused)
	{
		// Paused
		KillTimer(plugin.hwndParent, 1);
		ReportCurrentSongStatus(Paused);
	}
	else if (isPlayingResult == Stopped)
	{
		// Stopped
		KillTimer(plugin.hwndParent, 1);
		ReportIdleStatus();
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_WA_IPC && lParam == IPC_CB_MISC && wParam == IPC_CB_MISC_STATUS)
    {
		UpdateRichPresenceDetails();
    }

    return CallWindowProc(lpWndProcOld, hwnd, message, wParam, lParam);
}

void UpdateInMemorySettingsFromDialogState(HWND hWndDlg)
{
    // Update in-memory settings based on dialog state
    HWND checkboxHwnd = GetDlgItem(hWndDlg, IDC_CHECK_DISPLAY_TITLE_IN_STATUS);
    int buttonState = Button_GetCheck(checkboxHwnd);
    if (buttonState == BST_CHECKED)
        pluginSettings.DisplayTitleInSettings = true;
    else
        pluginSettings.DisplayTitleInSettings = false;

    HWND editboxHwnd = GetDlgItem(hWndDlg, IDC_EDIT_DISCORD_APPLICATION_ID);
    char stringData[255] = {};
    if (GetWindowTextA(editboxHwnd, stringData, _countof(stringData)) > 0)
    {
        pluginSettings.ApplicationID = stringData;
    }
}

void OnConfirmSettingsDialog(HWND hWndDlg)
{
    PluginSettings previousSettings = pluginSettings;

    UpdateInMemorySettingsFromDialogState(hWndDlg);

    bool applicationIDChanged = previousSettings.ApplicationID != pluginSettings.ApplicationID;
    bool displayTitleSettingChanged = previousSettings.DisplayTitleInSettings != pluginSettings.DisplayTitleInSettings;

    if (!applicationIDChanged && !displayTitleSettingChanged)
        return; // Nothing to do

    // Save settings to file
    SaveSettingsFile();

    bool shouldUpdateRichPresenceDetails = false;

    // The setting to display your currently-playing title has changed. Update Discord rich presence.
    if (displayTitleSettingChanged)
        shouldUpdateRichPresenceDetails = true;

    // The discord application ID has changed. Re-initialize everything against the new ID.
    if (previousSettings.ApplicationID != pluginSettings.ApplicationID)
    {
        ShutdownDiscord();
        InitDiscord();

        shouldUpdateRichPresenceDetails = true;
    }

    if (shouldUpdateRichPresenceDetails)
        UpdateRichPresenceDetails();
}

void PopulateSettingsDialogFields(HWND hWndDlg)
{
    HWND checkboxHwnd = GetDlgItem(hWndDlg, IDC_CHECK_DISPLAY_TITLE_IN_STATUS);

    if (pluginSettings.DisplayTitleInSettings)
        Button_SetCheck(checkboxHwnd, BST_CHECKED);
    else
        Button_SetCheck(checkboxHwnd, BST_UNCHECKED);

    HWND editboxHwnd = GetDlgItem(hWndDlg, IDC_EDIT_DISCORD_APPLICATION_ID);
    SetWindowTextA(editboxHwnd, pluginSettings.ApplicationID.c_str());
}

// Dialogue box callback function
BOOL CALLBACK ConfigDialogProc(HWND hWndDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
    {
        case WM_INITDIALOG:
        {
            if (GetWindowLong(hWndDlg, GWL_STYLE) & WS_CHILD)
            {
                // do nothing if this is a child window (tab page) callback, pass to the parent
                return FALSE;
            }

            PopulateSettingsDialogFields(hWndDlg);
            return TRUE;
        }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDOK:
            {
                OnConfirmSettingsDialog(hWndDlg);
                EndDialog(hWndDlg, 0);
                return TRUE;
                break;
            }
            case IDCANCEL:
            {
                EndDialog(hWndDlg, 0);
                return TRUE;
                break;
            }
        }
    }

    return FALSE;    // message not processed
}

void config() 
{
    DialogBox(plugin.hDllInstance, (LPTSTR)IDD_DIALOG_CONFIG, plugin.hwndParent, &ConfigDialogProc);
}

void quit() 
{
    ShutdownDiscord();
}

// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() 
{
    return &plugin;
}