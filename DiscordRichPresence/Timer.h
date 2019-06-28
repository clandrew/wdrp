#pragma once

class Timer
{
	bool m_initialized;
	bool m_isTimerAlive;
	UINT m_interval;
	HWND m_winampClientWindow;
	TIMERPROC m_timerProc;

public:
	Timer()
		: m_initialized(false)
		, m_isTimerAlive(false)
		, m_interval(2000)
		, m_winampClientWindow{}
	{
	}

	void Initialize(HWND winampClientWindow, TIMERPROC timerProc)
	{
		m_winampClientWindow = winampClientWindow;
		m_timerProc = timerProc;
		m_initialized = true;
	}

	void Set()
	{
		assert(m_initialized);

		if (m_isTimerAlive)
			return; // Nothing to do

		m_isTimerAlive = true;
		SetTimer(m_winampClientWindow, 1, m_interval, m_timerProc);
	}

	void Stop()
	{
		assert(m_initialized);

		if (!m_isTimerAlive)
			return; // nothing to do

		m_isTimerAlive = false;
		KillTimer(m_winampClientWindow, 1);
	}
};