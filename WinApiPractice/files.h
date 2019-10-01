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

enum FileOffset : UINT
{
	WindowWidthOffset = 0,
	WindowHeightOffset = 4,
	GridSizeOffset = 8,

	BackgroundColorOffset = 12,
	GridColorOffset = 16,

	IconFileOffset = 128,
	CursorFileOffset = 192,
};

const TCHAR* configFile = _T("C:\\Projects\\WinApiPractice\\WinApiPractice\\config.ini");
const UINT BUFFSIZE = 256;
const UINT FILE_MAP_START = 0;

template <typename T>
T* ReadMappingValue(LPVOID Mapping, FileOffset Offset)
{
	return (T*)((char*)Mapping + Offset);
}

inline Preferences* ReadPreferencesFromMapping(LPVOID Mapping)
{
	Preferences* prefs = new Preferences;
	
	prefs->WindowWidth = *ReadMappingValue<UINT>(Mapping, WindowWidthOffset);
	prefs->WindowHeight = *ReadMappingValue<UINT>(Mapping, WindowHeightOffset);
	prefs->GridSize = *ReadMappingValue<UINT>(Mapping, GridSizeOffset);

	prefs->BackgroundColor = *ReadMappingValue<COLORREF>(Mapping, BackgroundColorOffset);
	prefs->GridColor = *ReadMappingValue<COLORREF>(Mapping, GridColorOffset);
	
	prefs->IconFile = ReadMappingValue<TCHAR>(Mapping, IconFileOffset);
	prefs->CursorFile = ReadMappingValue<TCHAR>(Mapping, CursorFileOffset);
	
	return prefs;
}

Preferences* ReadConfigFromFileMapping()
{
	HANDLE hMapFile;      // handle for the file's memory-mapped region
	HANDLE hFile;         // the file handle
	BOOL bFlag;           // a result holder
	DWORD dBytesWritten;  // number of bytes written
	DWORD dwFileSize;     // temporary storage for file sizes
	DWORD dwFileMapSize;  // size of the file mapping
	DWORD dwMapViewSize;  // the size of the view
	DWORD dwFileMapStart; // where to start the file map view
	DWORD dwSysGran;      // system allocation granularity
	SYSTEM_INFO SysInfo;  // system information; used to get granularity
	LPVOID lpMapAddress;  // pointer to the base address of the memory-mapped region
	char* pData;          // pointer to the data
	int iData;            // on success contains the first int of data
	int iViewDelta;       // the offset into the view where the data shows up

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

	GetSystemInfo(&SysInfo);
	dwSysGran = SysInfo.dwAllocationGranularity;

	dwFileMapStart = (FILE_MAP_START / dwSysGran) * dwSysGran;
	_tprintf(TEXT("The file map view starts at %ld bytes into the file.\n"),
		dwFileMapStart);

	dwMapViewSize = (FILE_MAP_START % dwSysGran) + BUFFSIZE;
	_tprintf(TEXT("The file map view is %ld bytes large.\n"),
		dwMapViewSize);

	dwFileMapSize = FILE_MAP_START + BUFFSIZE;
	_tprintf(TEXT("The file mapping object is %ld bytes large.\n"),
		dwFileMapSize);

	iViewDelta = FILE_MAP_START - dwFileMapStart;
	_tprintf(TEXT("The data is %d bytes into the view.\n"),
		iViewDelta);

	dwFileSize = GetFileSize(hFile, NULL);
	_tprintf(TEXT("hFile size: %10d\n"), dwFileSize);

	hMapFile = CreateFileMapping(hFile,
		NULL,
		PAGE_READWRITE,
		0,
		BUFFSIZE,
		NULL);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("hMapFile is NULL: last error: %d\n"), GetLastError());
		return NULL;
	}

	lpMapAddress = MapViewOfFile(hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		dwFileMapStart,
		dwFileMapSize);
	if (lpMapAddress == NULL)
	{
		_tprintf(TEXT("lpMapAddress is NULL: last error: %d\n"), GetLastError());
		return NULL;
	}

	// TODO: Do not forget to delete this!
	Preferences* prefs = NULL;
	__try
	{
		prefs = ReadPreferencesFromMapping(lpMapAddress);
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

inline Preferences* ReadConfigFile(ReadingMethod Method)
{
	switch (Method)
	{
		case ReadingMethod::FileMapping:
			return ReadConfigFromFileMapping();
	}
	return NULL;
}

UINT GetDimension();
UINT GetHorizontalSize();
UINT GetVerticalSize();
COLORREF GetBackgroundColor();
COLORREF GetGridColor();
HICON GetApplicationIcon();
HCURSOR GetCursorIcon();