#include "winapi.h"
#include "preferences.h"

UINT BufferSize = 1024;

Preferences* ReadConfigUsingWinApi(const TCHAR* configFile)
{
	HANDLE hFile = CreateFile(configFile,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	TCHAR* buffer = new TCHAR[BufferSize];
	ReadFile(hFile, buffer, BufferSize, NULL, NULL);

	CloseHandle(hFile);

	Preferences* prefs = StringToPreferences(buffer);

	delete[] buffer;
	return prefs;
}

void WriteConfigUsingWinApi(const TCHAR* configFile, const Preferences* prefs)
{
	HANDLE hFile = CreateFile(configFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	const TCHAR* buffer = PreferencesToString(prefs);
	const UINT len = wcslen(buffer) * sizeof(wchar_t);
	WriteFile(hFile, buffer, len, NULL, NULL);

	delete[] buffer;
	CloseHandle(hFile);
}
