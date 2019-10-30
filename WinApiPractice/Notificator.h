#pragma once
#include <Windows.h>
#include <string>

class Notificator
{
public:
	void Create(const std::wstring&& pipeName);
	void Close();

	void Send(const std::wstring&& message) const;
	[[nodiscard]] std::wstring Receive() const;

private:
	HANDLE handle = nullptr;
	bool isCreated = false;
};
