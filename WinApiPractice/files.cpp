#include <Windows.h>
#include "preferences.h"
#include "filemapping.h"
#include "files.h"
#include <tchar.h>

const TCHAR* configFile = _T("C:\\Projects\\WinApiPractice\\WinApiPractice\\config.ini");

void WriteConfigFile(ReadingMethod method, Preferences* prefs)
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

Preferences* ReadConfigFile(ReadingMethod method)
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