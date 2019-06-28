#pragma once

enum PlaybackState // Enumeration values set up to match IPC_ISPLAYING
{
	Stopped = 0,
	Playing = 1,
	Paused = 3
};
