#include <Windows.h>
#include "preferences.h"
#include "filemapping.h"
#include "files.h"
#include <tchar.h>
#include "filestream.h"
#include "winapi.h"

const TCHAR* configFile = _T("C:\\Projects\\WinApiPractice\\WinApiPractice\\config.ini");

void WriteConfigFile(ReadingMethod method, Preferences* prefs)
{
	switch (method)
	{
		case ReadingMethod::FileMapping:
			WriteConfigToFileMapping(configFile, prefs);
			break;
		case ReadingMethod::Variables: break;
		case ReadingMethod::Streams:
			WriteConfigUsingStream(configFile, prefs);
			break;
		case ReadingMethod::WinApi:
			WriteConfigUsingWinApi(configFile, prefs);
			break;
		default:
			_tprintf(L"Please specify config file writing method");
	}
}

Preferences* ReadConfigFile(ReadingMethod method)
{
	switch (method)
	{
		case ReadingMethod::FileMapping:
			return ReadConfigFromFileMapping(configFile);
		case ReadingMethod::Variables: break;
		case ReadingMethod::Streams:
			return ReadConfigUsingStream(configFile);
		case ReadingMethod::WinApi:
			return ReadConfigUsingWinApi(configFile);
		default:
			_tprintf(L"Please specify config file reading method");
	}
	return NULL;
}
