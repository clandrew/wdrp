#include "stdafx.h"
#include "PresenceInfo.h"
#include "DiscordRichPresence.h"
#include "SettingsFile.h"

PresenceInfo::PresenceInfo()
	: m_initializeFn{}
	, m_shutdownFn{}
	, m_updatePresenceFn{}
	, m_runCallbacksFn{}
	, m_hDiscordModule{}
{
	memset(&m_presence, 0, sizeof(m_presence));
	m_presence.largeImageKey = "winamp-logo";
	m_presence.instance = 1;
	CurrentPlaybackState = Stopped;
	m_lowLevelTrackTitleIsUrl = false;
}

void PresenceInfo::SetStateText(char const* str)
{
	m_stateBuffer = str;
	m_presence.state = m_stateBuffer.c_str();
}

void PresenceInfo::SetLowLevelTrackTitle(char const* str)
{
	m_lowLevelTrackTitleBuffer = str;
	m_lowLevelTrackTitleIsUrl = _strnicmp(str, "http://", 7) == 0 || _strnicmp(str, "https://", 8) == 0;
	m_streamingTrackTitleBuffer.clear();
}

bool PresenceInfo::IsStreaming() const
{
	return m_lowLevelTrackTitleIsUrl;
}

void PresenceInfo::SetStreamingTrackTitle(char const* str)
{
	m_streamingTrackTitleBuffer = str;
}

void PresenceInfo::ClearDetails()
{
	m_lowLevelTrackTitleBuffer.clear();
	m_lowLevelTrackTitleIsUrl = false;

	m_streamingTrackTitleBuffer.clear();

	m_presence.details = nullptr;
}

void PresenceInfo::SetStartTimestamp(__int64 timestamp)
{
	m_presence.startTimestamp = timestamp;
}

void PresenceInfo::PostToDiscord()
{
	if (m_streamingTrackTitleBuffer.length() > 0) // Streaming title takes precedence
	{
		m_presence.details = m_streamingTrackTitleBuffer.c_str();
	}
	else
	{
		m_presence.details = m_lowLevelTrackTitleBuffer.c_str();
	}

	m_updatePresenceFn(&m_presence);
	m_runCallbacksFn();
}

bool PresenceInfo::HasDiscordModuleLoaded() const
{
	return m_hDiscordModule;
}

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

void PresenceInfo::InitializeDiscordRPC()
{
	if (g_pluginSettings.ApplicationID == "0")
		return;

	if (!m_hDiscordModule)
	{
		m_hDiscordModule = LoadLibrary(L"Plugins\\DiscordRichPresence\\discord-rpc.dll");
		if (!m_hDiscordModule)
			return;

		m_initializeFn = (Discord_InitializeFn)GetProcAddress(m_hDiscordModule, "Discord_Initialize");
		m_shutdownFn = (Discord_ShutdownFn)GetProcAddress(m_hDiscordModule, "Discord_Shutdown");
		m_updatePresenceFn = (Discord_UpdatePresenceFn)GetProcAddress(m_hDiscordModule, "Discord_UpdatePresence");
		m_runCallbacksFn = (Discord_RunCallbacksFn)GetProcAddress(m_hDiscordModule, "Discord_RunCallbacks");
	}

	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.joinGame = handleDiscordJoinGame;
	handlers.spectateGame = handleDiscordSpectateGame;
	handlers.joinRequest = handleDiscordJoinRequest;

	std::string applicationID = g_pluginSettings.ApplicationID.c_str();
	int autoRegister = 1;
	const char* noSteamID = nullptr;
	m_initializeFn(applicationID.c_str(), &handlers, autoRegister, noSteamID);
	m_runCallbacksFn();
}

void PresenceInfo::ShutdownDiscordRPC()
{
	if (m_hDiscordModule)
	{
		m_shutdownFn();
	}
}
