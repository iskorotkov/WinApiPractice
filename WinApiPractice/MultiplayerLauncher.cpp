#include "MultiplayerLauncher.h"
#include "Windows.h"
#include <string>

void MultiplayerLauncher::LaunchClient() const
{
	wchar_t filename[MAX_PATH];
	GetModuleFileName(nullptr, filename, MAX_PATH);

	wcscat_s(filename, L" -filemapping -client");

	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	const auto result = CreateProcess(
		nullptr,
		filename,
		nullptr,
		nullptr,
		FALSE,
		0,
		nullptr,
		nullptr,
		&sInfo,
		&pInfo
	);
}
