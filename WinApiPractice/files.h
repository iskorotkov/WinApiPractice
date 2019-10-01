#pragma once
#include <Windows.h>
#include <tchar.h>
#include "preferences.h"

enum class ReadingMethod
{
	Unknown,
	FileMapping,
	Variables,
	Streams,
	WinApi,
};

const TCHAR* configFile = _T("C:\\Projects\\WinApiPractice\\WinApiPractice\\config.ini");

const UINT BUFFSIZE = 1024;
const UINT FILE_MAP_START = 0;

HANDLE hMapFile;     // handle for the file's memory-mapped region
HANDLE hFile;        // the file handle
LPVOID lpMapAddress; // pointer to the base address of the memory-mapped region

Preferences* ReadConfigFromFileMapping()
{
	hFile = CreateFile(configFile,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			configFile);
		return NULL;
	}

	SYSTEM_INFO SysInfo; // system information; used to get granularity
	GetSystemInfo(&SysInfo);
	DWORD dwSysGran = SysInfo.dwAllocationGranularity;

	DWORD dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
	_tprintf(TEXT("The file map view starts at %ld bytes into the file.\n"),
		dwFileMapStart);

	DWORD dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
	_tprintf(TEXT("The file map view is %ld bytes large.\n"),
		dwMapViewSize);

	DWORD dwFileMapSize = FILE_MAP_START + BUFFSIZE;
	_tprintf(TEXT("The file mapping object is %ld bytes large.\n"),
		dwFileMapSize);

	int iViewDelta = FILE_MAP_START - dwFileMapStart;
	_tprintf(TEXT("The data is %d bytes into the view.\n"),
		iViewDelta);

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_tprintf(TEXT("hFile size: %10d\n"), dwFileSize);

	hMapFile = CreateFileMapping(hFile,
		NULL,
		PAGE_READWRITE,
		0,
		dwFileMapSize,
		NULL);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		return NULL;
	}

	lpMapAddress = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		dwMapViewSize);
	if (lpMapAddress == NULL)
	{
		_tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		return NULL;
	}

	// TODO: Do not forget to delete this!
	Preferences* prefs = NULL;
	__try
	{
		prefs = StringToPreferences((TCHAR*)lpMapAddress);
	}
	__except (GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		// Failed to read from the view.
	}
	return prefs;
}

void WriteConfigToFileMapping(Preferences* prefs)
{
	if (prefs == NULL || lpMapAddress == NULL)
	{
		_tprintf(L"Preferences are NULL or file mapping is not created.");
	}
	const TCHAR* content = PreferencesToString(prefs);

	wcscpy_s((TCHAR*)lpMapAddress, BUFFSIZE, content);

	// FlushViewOfFile(lpMapAddress, wcslen(content));
	UnmapViewOfFile(lpMapAddress);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
}

inline Preferences* ReadConfigFile(ReadingMethod Method)
{
	switch (Method)
	{
		case ReadingMethod::FileMapping:
			return ReadConfigFromFileMapping();
	}
	return NULL;
}
