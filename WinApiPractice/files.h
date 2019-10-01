#pragma once
#include <Windows.h>
#include <tchar.h>
#include "preferences.h"
#include "filemapping.h"

enum class ReadingMethod
{
	Unknown,
	FileMapping,
	Variables,
	Streams,
	WinApi,
};

const TCHAR* configFile = _T("C:\\Projects\\WinApiPractice\\WinApiPractice\\config.ini");

inline void WriteConfigFile(ReadingMethod method, Preferences* prefs)
{
	switch (method)
	{
		case ReadingMethod::FileMapping:
			WriteConfigToFileMapping(configFile, prefs);
			break;
		case ReadingMethod::Unknown: break;
		case ReadingMethod::Variables: break;
		case ReadingMethod::Streams: break;
		case ReadingMethod::WinApi: break;
		default: ;
	}
}

inline Preferences* ReadConfigFile(ReadingMethod method)
{
	switch (method)
	{
		case ReadingMethod::FileMapping:
			return ReadConfigFromFileMapping(configFile);
		case ReadingMethod::Unknown: break;
		case ReadingMethod::Variables: break;
		case ReadingMethod::Streams: break;
		case ReadingMethod::WinApi: break;
		default: ;
	}
	return NULL;
}
