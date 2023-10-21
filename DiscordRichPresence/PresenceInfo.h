#pragma once

enum PlaybackState;

class PresenceInfo
{
	DiscordRichPresence m_presence;
	std::string m_stateBuffer;

	std::string m_lowLevelTrackTitleBuffer;
	bool m_lowLevelTrackTitleIsUrl;

	std::string m_streamingTrackTitleBuffer;

	HMODULE m_hDiscordModule;
	//used to interop with Discord callbacks for connected status
	static bool m_discordConnected;

	typedef void (DISCORD_EXPORT *Discord_InitializeFn)(const char*, DiscordEventHandlers*, int, const char*);
	typedef void (DISCORD_EXPORT *Discord_ShutdownFn)(void);
	typedef void (DISCORD_EXPORT *Discord_UpdatePresenceFn)(const DiscordRichPresence*);
	typedef void (DISCORD_EXPORT *Discord_RunCallbacksFn)(void);

	Discord_InitializeFn m_initializeFn;
	Discord_ShutdownFn m_shutdownFn;
	Discord_UpdatePresenceFn m_updatePresenceFn;
	Discord_RunCallbacksFn m_runCallbacksFn;

public:
	PresenceInfo();
	PlaybackState CurrentPlaybackState;

	//used to interop with Discord callbacks for connected status
	static void SetDiscordRPCConnectStatus(bool);
	bool IsDiscordRPCConnected();
	void InitializeDiscordRPC();
	void ShutdownDiscordRPC();

	bool HasDiscordModuleLoaded() const;

	void SetStateText(char const* str);

	void SetLowLevelTrackTitle(char const* str);

	bool IsStreaming() const;
	void SetStreamingTrackTitle(char const* str);

	void ClearDetails();
	void SetStartTimestamp(__int64 timestamp);

	void PostToDiscord();
};