#pragma once
#include <Windows.h>

struct Preferences* ReadConfigUsingStream(const TCHAR* configFile);

void WriteConfigUsingStream(const TCHAR* configFile, const Preferences* prefs);
