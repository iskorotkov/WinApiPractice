#include "MultiplayerLauncher.h"
#include "Windows.h"

MultiplayerLauncher::MultiplayerLauncher()
{
	storage.Open(L"ClientsCounter", sizeof(ClientsCounter));
	counter = reinterpret_cast<ClientsCounter*>(storage.GetStorage());

	if (counter->clients >= 2)
	{
		throw std::exception("There are already two instances launched.\nFinish old games before starting new ones.");
	}
	++counter->clients;
}

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
