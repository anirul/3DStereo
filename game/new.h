/////////////////////////////////////////////////////////////////////
// miniRT New (header)
/////////////////////////////////////////////////////////////////////
// author	: Dubouchet frederic
// e-mail	: angel@calodox.org
/////////////////////////////////////////////////////////////////////

#ifndef __MINIRT_NEW_HEADER__
#define __MINIRT_NEW_HEADER__

void* operator new (size_t nbrByte);
void* operator new [] (size_t nbrByte);

void operator delete (void* ptr);
void operator delete [] (void* ptr);

#endif // __MINIRT_NEW_HEADER__