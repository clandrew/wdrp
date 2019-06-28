#pragma once

enum PlaybackState;

class PresenceInfo
{
	DiscordRichPresence m_presence;
	std::string m_stateBuffer;
	std::string m_detailsBuffer;

	HMODULE m_hDiscordModule;

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

	void InitializeDiscordRPC();
	void ShutdownDiscordRPC();

	bool HasDiscordModuleLoaded() const;

	void SetStateText(char const* str);
	void SetDetails(char const* str);
	void ClearDetails();
	void SetStartTimestamp(__int64 timestamp);

	void PostToDiscord();
};