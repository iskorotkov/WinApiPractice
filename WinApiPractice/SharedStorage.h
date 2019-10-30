#pragma once
#include "Windows.h"
#include <string>

class SharedStorage
{
public:
	explicit SharedStorage() = default;

	SharedStorage(const SharedStorage& other) = default;
	SharedStorage(SharedStorage&& other) noexcept = default;
	SharedStorage& operator=(const SharedStorage& other) = default;
	SharedStorage& operator=(SharedStorage&& other) noexcept = default;

	~SharedStorage();

	void Open(const std::wstring&& name, int size);
	void Close();

	[[nodiscard]] std::byte* GetStorage() const;

private:
	bool isOpened = false;
	LPVOID mappingAddress = nullptr;
	HANDLE mappingHandle = nullptr;
};
