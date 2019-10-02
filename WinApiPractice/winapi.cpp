#include "winapi.h"
#include "preferences.h"

Preferences* ReadConfigUsingWinApi(const TCHAR* configFile)
{
	const HANDLE hFile = CreateFile(configFile,
		GENERIC_READ,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	const UINT bufferSize = GetFileSize(hFile, nullptr);
	TCHAR* buffer = new TCHAR[bufferSize];
	ReadFile(hFile, buffer, bufferSize, nullptr, nullptr);

	CloseHandle(hFile);

	Preferences* prefs = StringToPreferences(buffer);

	delete[] buffer;
	return prefs;
}

void WriteConfigUsingWinApi(const TCHAR* configFile, const Preferences* prefs)
{
	const HANDLE hFile = CreateFile(configFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	const TCHAR* buffer = PreferencesToString(prefs);
	const UINT len = wcslen(buffer) * sizeof(wchar_t);
	WriteFile(hFile, buffer, len, nullptr, nullptr);

	CloseHandle(hFile);
	delete[] buffer;
}
