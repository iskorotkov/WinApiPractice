#pragma once
#include <Windows.h>

struct Preferences* ReadConfigUsingVariables(const TCHAR* configFile);

void WriteConfigUsingVariables(const TCHAR* configFile, const Preferences* prefs);
