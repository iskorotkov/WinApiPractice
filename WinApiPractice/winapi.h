#pragma once
#include <Windows.h>

struct Preferences* ReadConfigUsingWinApi(const TCHAR* configFile);

void WriteConfigUsingWinApi(const TCHAR* configFile, const Preferences* prefs);
