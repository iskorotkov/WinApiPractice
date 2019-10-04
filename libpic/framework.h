#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

extern "C" __declspec(dllexport) char* load_image(const char* filename, int* width, int* height);
