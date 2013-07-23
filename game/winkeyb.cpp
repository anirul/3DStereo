#include "main.h"

bool down(int key) {
	return (GetAsyncKeyState(key) & 0x8000) ? true : false;
}

bool up(int key) {
	return !down(key);
}