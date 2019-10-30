#include "Notificator.h"

void Notificator::Create(const std::wstring&& pipeName)
{
	if (isCreated)
	{
		return;
	}

	const auto fullName = LR"(\\.\pipe\)" + pipeName;
	handle = CreateFile(
		fullName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (!handle)
	{
		// TODO: Never executed code!
		handle = CreateNamedPipe(
			fullName.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			0,
			0,
			0,
			nullptr
		);

		if (!handle)
		{
			// TODO
		}
	}
	isCreated = true;
}

void Notificator::Send(const std::wstring&& message) const
{
	if (!isCreated)
	{
		// TODO:
	}

	WriteFile(
		handle,
		message.c_str(),
		message.length() * sizeof(wchar_t),
		nullptr,
		nullptr
	);
}

std::wstring Notificator::Receive() const
{
	if (!isCreated)
	{
		// TODO:
	}

	const auto allocated = 512;
	const auto buffer = new wchar_t[allocated];
	auto bytesRead = 0ul;
	
	ReadFile(
		handle,
		buffer,
		allocated,
		&bytesRead,
		nullptr
	);

	const std::wstring ws(buffer, bytesRead);
	delete[] buffer;
	return ws;
}

void Notificator::Close()
{
	if (!isCreated)
	{
		return;
	}


	// TODO: Should I flush file buffers here?
	FlushFileBuffers(handle);
	
	CloseHandle(handle);

	isCreated = false;
}
