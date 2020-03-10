// DiscordRichPresence.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DiscordRichPresence.h"
#include "Resource.h"
#include "PresenceInfo.h"
#include "SettingsFile.h"
#include "Timer.h"

// Forward declares
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void ReportIdleStatus();
void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD);

// plugin version (don't touch this)
#define GPPHDR_VER 0x10

// plugin name/title (change this to something you like)
char PLUGIN_NAME[] = "Discord Rich Presence";

// Variables
WNDPROC g_lpWndProcOld = 0;
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
winampGeneralPurposePlugin g_plugin = {
    GPPHDR_VER,  // version of the plugin, defined in "gen_myplugin.h"
    PLUGIN_NAME, // name/title of the plugin, defined in "gen_myplugin.h"
    init,        // function name which will be executed on init event
    config,      // function name which will be executed on config event
    quit,        // function name which will be executed on quit event
    0,           // handle to Winamp main window, loaded by winamp when this dll is loaded
    0            // hinstance to this dll, loaded by winamp when this dll is loaded
}; 

Timer g_timer;
PresenceInfo g_presenceInfo;

void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
	assert(g_presenceInfo.CurrentPlaybackState == Playing);
	assert(g_pluginSettings.ShowElapsedTime || g_presenceInfo.IsStreaming());

	if (!g_presenceInfo.HasDiscordModuleLoaded())
		return;

	if (g_presenceInfo.IsStreaming()) // Streaming requires more frequent title updates
	{
		int curPos = SendMessage(g_plugin.hwndParent, WM_WA_IPC, 0, IPC_GETLISTPOS);
		char* playlistTitle = (char*)SendMessage(g_plugin.hwndParent, WM_WA_IPC, curPos, IPC_GETPLAYLISTTITLE);
		g_presenceInfo.SetStreamingTrackTitle(playlistTitle);
	}

	if (g_pluginSettings.ShowElapsedTime)
	{
		int currentPlaybackPositionInMSMode = 0;
		int playbackPosition = SendMessage(g_plugin.hwndParent, WM_WA_IPC, currentPlaybackPositionInMSMode, IPC_GETOUTPUTTIME);
		int playbackPositionInSeconds = playbackPosition / 1000;

		std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
		long long dtn = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();

		g_presenceInfo.SetStartTimestamp(dtn - playbackPositionInSeconds);
	}

	g_presenceInfo.PostToDiscord();
}

int init() 
{
    if (IsWindowUnicode(g_plugin.hwndParent))
        g_lpWndProcOld = (WNDPROC)SetWindowLongW(g_plugin.hwndParent, GWL_WNDPROC, (LONG)WndProc);
    else
        g_lpWndProcOld = (WNDPROC)SetWindowLongA(g_plugin.hwndParent, GWL_WNDPROC, (LONG)WndProc);

	g_timer.Initialize(g_plugin.hwndParent, TimerProc);

    LoadSettingsFile();
	
	if (g_pluginSettings.SendWhenPausedOrStopped)
	{
		g_presenceInfo.InitializeDiscordRPC();

    	ReportIdleStatus();
	}

    return 0;
}

void ReportIdleStatus()
{
    if (g_pluginSettings.ApplicationID == "0")
        return;

	if (!g_presenceInfo.IsDiscordRPCConnected())
	{
        g_presenceInfo.InitializeDiscordRPC();
	}

	g_presenceInfo.CurrentPlaybackState = Stopped;
	g_presenceInfo.SetStartTimestamp(0);
	g_presenceInfo.SetStateText("(Idle)");
	g_presenceInfo.ClearDetails();
	g_presenceInfo.PostToDiscord();	
}

void ReportCurrentSongStatus(PlaybackState playbackState)
{
	assert(playbackState != Stopped);

    if (g_pluginSettings.ApplicationID == "0")
        return;

	if (!g_presenceInfo.IsDiscordRPCConnected())
	{
        g_presenceInfo.InitializeDiscordRPC();
	}
	
	g_presenceInfo.CurrentPlaybackState = playbackState;
	g_presenceInfo.SetStartTimestamp(0);
	g_presenceInfo.SetStateText(playbackState == Playing ? "(Playing)" : "(Paused)");
	
    std::string detailsMessage;
    if (g_pluginSettings.DisplayTitleInStatus)
    {
		std::wstring title = (wchar_t *)SendMessage(g_plugin.hwndParent, WM_WA_IPC, 0, IPC_GET_PLAYING_TITLE);
		std::wstring_convert<std::codecvt_utf8<wchar_t>> convertWideToUTF8;
		detailsMessage = convertWideToUTF8.to_bytes(title);
    }
    else
    {
        detailsMessage = "";
    }
	g_presenceInfo.SetLowLevelTrackTitle(detailsMessage.c_str());
	g_presenceInfo.PostToDiscord();
}

void UpdateRichPresenceDetails()
{
	LONG isPlayingResult = SendMessage(g_plugin.hwndParent, WM_WA_IPC, 0, IPC_ISPLAYING);

	if (isPlayingResult == Playing)
	{
		ReportCurrentSongStatus(Playing);
		if (g_pluginSettings.ShowElapsedTime || g_presenceInfo.IsStreaming())
		{
			g_timer.Set();
		}
		else
		{
			g_timer.Stop();
		}
	}
	else if (isPlayingResult == Paused)
	{
		g_timer.Stop();
		if (g_pluginSettings.SendWhenPausedOrStopped)
		{
			ReportCurrentSongStatus(Paused);
		}
		else 
		{
			g_presenceInfo.ShutdownDiscordRPC();
		}
	}
	else if (isPlayingResult == Stopped)
	{
		g_timer.Stop();
		if (g_pluginSettings.SendWhenPausedOrStopped)
		{
			ReportIdleStatus();
		}
		else 
		{
			g_presenceInfo.ShutdownDiscordRPC();
		}
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_WA_IPC && lParam == IPC_CB_MISC && wParam == IPC_CB_MISC_STATUS)
    {
		// Notification sent from Winamp on any change in playback.
		UpdateRichPresenceDetails();
    }

    return CallWindowProc(g_lpWndProcOld, hwnd, message, wParam, lParam);
}

void UpdateInMemorySettingsFromDialogState(HWND hWndDlg)
{
    // Update in-memory settings based on dialog state
    HWND checkboxHwnd = GetDlgItem(hWndDlg, IDC_CHECK_DISPLAY_TITLE_IN_STATUS);
	g_pluginSettings.DisplayTitleInStatus = Button_GetCheck(checkboxHwnd) == BST_CHECKED;

	checkboxHwnd = GetDlgItem(hWndDlg, IDC_SHOW_ELAPSED_TIME);
	g_pluginSettings.ShowElapsedTime = Button_GetCheck(checkboxHwnd) == BST_CHECKED;

    HWND editboxHwnd = GetDlgItem(hWndDlg, IDC_EDIT_DISCORD_APPLICATION_ID);
    char stringData[255] = {};
    if (GetWindowTextA(editboxHwnd, stringData, _countof(stringData)) > 0)
    {
        g_pluginSettings.ApplicationID = stringData;
    }

	checkboxHwnd = GetDlgItem(hWndDlg, IDC_SEND_WHEN_PAUSE_OR_STOPPED);
	g_pluginSettings.SendWhenPausedOrStopped = Button_GetCheck(checkboxHwnd) == BST_CHECKED;
}

void OnConfirmSettingsDialog(HWND hWndDlg)
{
    PluginSettings previousSettings = g_pluginSettings;

    UpdateInMemorySettingsFromDialogState(hWndDlg);

    bool applicationIDChanged = previousSettings.ApplicationID != g_pluginSettings.ApplicationID;
    bool displayTitleSettingChanged = previousSettings.DisplayTitleInStatus != g_pluginSettings.DisplayTitleInStatus;
	bool elapsedTimeChanged = previousSettings.ShowElapsedTime != g_pluginSettings.ShowElapsedTime;
	bool sendOnPausedOrStopChanged = previousSettings.SendWhenPausedOrStopped != g_pluginSettings.SendWhenPausedOrStopped;

    if (!applicationIDChanged && !displayTitleSettingChanged && !elapsedTimeChanged && !sendOnPausedOrStopChanged)
        return; // Nothing to do

    // Save settings to file
    SaveSettingsFile();

    bool shouldUpdateRichPresenceDetails = false;

    // The setting to display your currently-playing title has changed. Update Discord rich presence.
	if (displayTitleSettingChanged || elapsedTimeChanged)
	{
		shouldUpdateRichPresenceDetails = true;
	}

    // The discord application ID has changed. Re-initialize everything against the new ID.
    if (previousSettings.ApplicationID != g_pluginSettings.ApplicationID)
    {
		g_presenceInfo.ShutdownDiscordRPC();
		g_presenceInfo.InitializeDiscordRPC();

        shouldUpdateRichPresenceDetails = true;
    }

	if (previousSettings.SendWhenPausedOrStopped != g_pluginSettings.SendWhenPausedOrStopped)
	{
		shouldUpdateRichPresenceDetails = true;
	}

	if (shouldUpdateRichPresenceDetails)
	{
		UpdateRichPresenceDetails();
	}
}

void PopulateSettingsDialogFields(HWND hWndDlg)
{
    HWND checkboxHwnd = GetDlgItem(hWndDlg, IDC_CHECK_DISPLAY_TITLE_IN_STATUS);
	if (g_pluginSettings.DisplayTitleInStatus)
	{
		Button_SetCheck(checkboxHwnd, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(checkboxHwnd, BST_UNCHECKED);
	}

	checkboxHwnd = GetDlgItem(hWndDlg, IDC_SHOW_ELAPSED_TIME);
	if (g_pluginSettings.ShowElapsedTime)
	{
		Button_SetCheck(checkboxHwnd, BST_CHECKED);
	}
	else
	{
		Button_SetCheck(checkboxHwnd, BST_UNCHECKED);
	}

    HWND editboxHwnd = GetDlgItem(hWndDlg, IDC_EDIT_DISCORD_APPLICATION_ID);
    SetWindowTextA(editboxHwnd, g_pluginSettings.ApplicationID.c_str());

	checkboxHwnd = GetDlgItem(hWndDlg, IDC_SEND_WHEN_PAUSE_OR_STOPPED);
	Button_SetCheck(checkboxHwnd, (g_pluginSettings.SendWhenPausedOrStopped ? BST_CHECKED : BST_UNCHECKED));
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
	case BN_CLICKED:
		{
			auto buttonIdentifier = LOWORD(wParam);
			if (buttonIdentifier == IDC_CHECK_DISPLAY_TITLE_IN_STATUS)
			{
				HWND displayAllTheThingsCheckbox = GetDlgItem(hWndDlg, IDC_CHECK_DISPLAY_TITLE_IN_STATUS);
				int buttonState = Button_GetCheck(displayAllTheThingsCheckbox);
				bool enableElapsedTime = buttonState == BST_CHECKED;

				HWND elapsedTimeCheckbox = GetDlgItem(hWndDlg, IDC_SHOW_ELAPSED_TIME);
				if (enableElapsedTime)
				{
					EnableWindow(elapsedTimeCheckbox, TRUE);
				}
				else
				{
					Button_SetCheck(elapsedTimeCheckbox, BST_UNCHECKED);
					EnableWindow(elapsedTimeCheckbox, FALSE);
				}
			}
		}
    }

    return FALSE;    // message not processed
}

void config() 
{
    DialogBox(g_plugin.hDllInstance, (LPTSTR)IDD_DIALOG_CONFIG, g_plugin.hwndParent, &ConfigDialogProc);
}

void quit() 
{
	g_presenceInfo.ShutdownDiscordRPC();
}

// This is an export function called by winamp which returns this plugin info.
// We wrap the code in 'extern "C"' to ensure the export isn't mangled if used in a CPP file.
extern "C" __declspec(dllexport) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin() 
{
    return &g_plugin;
}