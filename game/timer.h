//////////////////////////////////////////////////////////////////////////
// timer Header
//////////////////////////////////////////////////////////////////////////
// author	: Frederic Dubouchet
// e-mail	: angel@calodox.org
//////////////////////////////////////////////////////////////////////////
// time handler. not portable yet

#ifndef __3DSTEREO_TIMER_HEADER_DEFINED__
#define __3DSTEREO_TIMER_HEADER_DEFINED__

class timer {
	LARGE_INTEGER m_tickpersecond;
	LARGE_INTEGER m_starttime;
public :
	timer();
	~timer() {}
	bool init();
	// get sec since last call
	float get_elapsed_seconds();
	// get sec since call to init
	float now();
	// return FPS
	float get_fps(unsigned int elapsedframe = 1);
};

#endif // __3DSTEREO_TIMER_HEADER_DEFINED__