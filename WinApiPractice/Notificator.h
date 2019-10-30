#pragma once
#include <Windows.h>
#include <string>

class [[deprecated("Unused class. I was wrong for creating and spending too much time on it. The solution was simpler.")]] Notificator
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
