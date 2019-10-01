#pragma once
#include <Windows.h>

struct Preferences* ReadConfigFromFileMapping(const TCHAR* configFile);

void WriteConfigToFileMapping(const TCHAR* configFile, Preferences* prefs);
