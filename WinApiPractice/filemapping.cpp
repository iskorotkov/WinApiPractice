#include <Windows.h>
#include "filemapping.h"
#include "preferences.h"
#include <tchar.h>

Preferences* ReadConfigFromFileMapping(const TCHAR* configFile)
{
	const HANDLE hFile = CreateFile(configFile,
		GENERIC_READ,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			configFile);
		return nullptr;
	}

	const UINT bufferSize = GetFileSize(hFile, nullptr);
	const UINT fileMapStart = 0;

	SYSTEM_INFO SysInfo; // system information; used to get granularity
	GetSystemInfo(&SysInfo);
	const DWORD dwSysGran = SysInfo.dwAllocationGranularity;

	const DWORD dwFileMapStart = fileMapStart / dwSysGran * dwSysGran;
	const DWORD dwMapViewSize = fileMapStart % dwSysGran + bufferSize;
	const DWORD dwFileMapSize = fileMapStart + bufferSize;

	const HANDLE hMapFile = CreateFileMapping(hFile,
		nullptr,
		PAGE_READONLY,
		0,
		dwFileMapSize,
		nullptr);
	if (hMapFile == nullptr)
	{
		_tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		return nullptr;
	}

	const LPVOID lpMapAddress = MapViewOfFile(hMapFile,
		FILE_MAP_READ,
		0,
		dwFileMapStart,
		dwMapViewSize);
	if (lpMapAddress == nullptr)
	{
		_tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		return nullptr;
	}

	// TODO: Do not forget to delete this!
	Preferences* prefs = nullptr;
	__try
	{
		prefs = StringToPreferences((TCHAR*)lpMapAddress);
	}
	__except (GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		// Failed to read from the view.
	}

	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(hFile);

	return prefs;
}

void WriteConfigToFileMapping(const TCHAR* configFile, Preferences* prefs)
{
	if (prefs == nullptr)
	{
		_tprintf(L"Preferences are NULL.");
		return;
	}

	const TCHAR* content = PreferencesToString(prefs);

	const UINT bufferSize = wcslen(content) * sizeof TCHAR;
	const UINT fileMapStart = 0;

	const HANDLE hFile = CreateFile(configFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			configFile);
		return;
	}

	SYSTEM_INFO SysInfo; // system information; used to get granularity
	GetSystemInfo(&SysInfo);
	const DWORD dwSysGran = SysInfo.dwAllocationGranularity;

	const DWORD dwFileMapStart = fileMapStart / dwSysGran * dwSysGran;
	const DWORD dwMapViewSize = fileMapStart % dwSysGran + bufferSize;
	const DWORD dwFileMapSize = fileMapStart + bufferSize;

	const HANDLE hMapFile = CreateFileMapping(hFile,
		nullptr,
		PAGE_READWRITE,
		0,
		dwFileMapSize,
		nullptr);
	if (hMapFile == nullptr)
	{
		_tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		return;
	}

	const LPVOID lpMapAddress = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		dwFileMapStart,
		dwMapViewSize);
	if (lpMapAddress == nullptr)
	{
		_tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		return;
	}

	wcscpy_s((TCHAR*)lpMapAddress, bufferSize, content);
	delete[] content;
	
	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
}
