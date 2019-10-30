#include "SharedStorage.h"

SharedStorage::~SharedStorage()
{
	Close();
}

void SharedStorage::Open(const std::wstring&& name, const int size)
{
	if (isOpened)
	{
		return;
	}

	if (size <= 0)
	{
		// TODO
		return;
	}

	const auto fileMapSize = size;
	mappingHandle = CreateFileMapping(INVALID_HANDLE_VALUE,
		nullptr,
		PAGE_READWRITE,
		0,
		fileMapSize,
		name.c_str());

	if (!mappingHandle)
	{
		// TODO
	}

	mappingAddress = MapViewOfFile(mappingHandle,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		fileMapSize);

	if (!mappingAddress)
	{
		// TODO
	}

	isOpened = true;
}

void SharedStorage::Close()
{
	if (!isOpened)
	{
		return;
	}

	UnmapViewOfFile(mappingAddress);
	CloseHandle(mappingHandle);

	isOpened = false;
}

std::byte* SharedStorage::GetStorage() const
{
	if (!mappingAddress)
	{
		// TODO
	}

	return static_cast<std::byte*>(mappingAddress);
}
