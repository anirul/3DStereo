/////////////////////////////////////////////////////////////////////
// miniRT New
/////////////////////////////////////////////////////////////////////
// author	: Dubouchet Frederic
// e-mail	: angel@calodox.org
/////////////////////////////////////////////////////////////////////

#include "new.h"
#include <stdio.h>
#include <malloc.h>

void* operator new (size_t nbrByte) {
#ifdef _DEBUG
//	printf("new call size %d\n", nbrByte);
#endif
#ifdef _M_IX86
	return _mm_malloc(nbrByte, 16);
#else
	return malloc(nbrByte);
#endif // _M_IX86
}

void* operator new [] (size_t nbrByte) {
#ifdef _DEBUG
//	printf("new [] call size %d\n", nbrByte);
#endif
#ifdef _M_IX86
	return _mm_malloc(nbrByte, 16);
#else
	return malloc(nbrByte);
#endif // _M_IX86
}

void operator delete (void* ptr) {
#ifdef _DEBUG
//	printf("delete pointer %p\n", ptr);
#endif
#ifdef _M_IX86
	_mm_free(ptr);
#else
	free(ptr);
#endif // _M_IX86
}

void operator delete [] (void* ptr) {
#ifdef _DEBUG
//	printf("delete [] pointer %p\n", ptr);
#endif
#ifdef _M_IX86
	_mm_free(ptr);
#else
	free(ptr);
#endif // _M_IX86
}
