#include "Notificator.h"
#include <iostream>

void Notificator::Create(const std::wstring&& pipeName)
{
	if (isCreated)
	{
		return;
	}

	const auto fullName = LR"(\\.\mailslot\)" + pipeName;
	handle = CreateFile(
		fullName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		OPEN_EXISTING,
		0,
		nullptr
	);

	if (!handle || handle == INVALID_HANDLE_VALUE)
	{
		//handle = CreateNamedPipe(
		//	fullName.c_str(),
		//	PIPE_ACCESS_DUPLEX,
		//	PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		//	PIPE_UNLIMITED_INSTANCES,
		//	1024,
		//	1024,
		//	10,
		//	nullptr
		//);
		handle = CreateMailslot(
			fullName.c_str(),
			0,
			MAILSLOT_WAIT_FOREVER,
			nullptr
		);
		isServer = true;

		const auto secondHandle = CreateFile(
			fullName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr
		);

		if (!handle || handle == INVALID_HANDLE_VALUE)
		{
			// TODO
		}
		std::cout << handle << '\n' << secondHandle;
	}
	isCreated = true;
}

void Notificator::Send(const std::wstring&& message) const
{
	if (!isCreated)
	{
		// TODO:
	}

	auto bytesWritten = 0ul;
	WriteFile(
		handle,
		message.c_str(),
		(message.length() + 1) * sizeof(wchar_t),
		&bytesWritten,
		nullptr
	);
	std::cout << "Bytes written: " << bytesWritten << '\n';
	std::wcout << "Written message: " << message << '\n';
}

std::wstring Notificator::Receive() const
{
	if (!isCreated)
	{
		// TODO:
	}

	auto messageSize = 0ul;
	auto messages = 0ul;
	const auto result = GetMailslotInfo(
		handle,
		0,
		&messageSize,
		&messages,
		nullptr
	);

	//const auto result = GetNamedPipeInfo(
	//	handle,
	//	nullptr,
	//	&messageSize,
	//	nullptr,
	//	nullptr
	//);

	if (!result)
	{
		// TODO
	}

	if (messageSize == MAILSLOT_NO_MESSAGE || messageSize <= 0)
	{
		return {};
	}

	const auto buffer = new wchar_t[messageSize];
	auto bytesRead = 0ul;
	ReadFile(
		handle,
		buffer,
		messageSize,
		&bytesRead,
		nullptr
	);
	std::cout << "Bytes read: " << bytesRead << '\n';

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
	if (isServer)
	{
		FlushFileBuffers(handle);
	}

	CloseHandle(handle);

	isCreated = false;
}
