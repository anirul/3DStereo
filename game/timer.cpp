#include "main.h"

timer::timer() {
	m_starttime.QuadPart = 0;
	m_tickpersecond.QuadPart = 0;
}

bool timer::init() {
	if (!QueryPerformanceFrequency(&m_tickpersecond))
		return false;
	QueryPerformanceCounter(&m_starttime);
	return true;
}

float timer::get_elapsed_seconds() {
	static LARGE_INTEGER s_lasttime = m_starttime;
	LARGE_INTEGER currenttime;
	QueryPerformanceCounter(&currenttime);
	float sec = ((float)currenttime.QuadPart - (float)s_lasttime.QuadPart) /
		(float)m_tickpersecond.QuadPart;
	s_lasttime = currenttime;
	return sec;
}

float timer::now() {
	LARGE_INTEGER currenttime;
	QueryPerformanceCounter(&currenttime);
	return ((float)currenttime.QuadPart - (float)m_starttime.QuadPart) /
		(float)m_tickpersecond.QuadPart;
}

float timer::get_fps(unsigned int elapsedframe /* = 1 */) {
	static LARGE_INTEGER s_lasttime = m_starttime;
	LARGE_INTEGER currenttime;
	QueryPerformanceCounter(&currenttime);
	float fps = (float)elapsedframe * (float)m_tickpersecond.QuadPart /
		((float)currenttime.QuadPart - (float) s_lasttime.QuadPart);
	s_lasttime = currenttime;
	return fps;
}