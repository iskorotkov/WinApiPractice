#include "SharedStorage.h"
#include <iostream>

SharedStorage::~SharedStorage()
{
	Close();
}

void SharedStorage::Open(const std::wstring& name, const int size)
{
	if (isOpened)
	{
		return;
	}

	if (size <= 0)
	{
		throw std::exception("Can't open shared storage with non-positive size.");
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
		throw std::exception("Unable to create file mapping.");
	}

	mappingAddress = MapViewOfFile(mappingHandle,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		fileMapSize);

	if (!mappingAddress)
	{
		throw std::exception("Unable to retrieve mapping address.");
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
	if (!isOpened)
	{
		throw std::exception("Trying to get storage content before creating the file mapping.");
	}
	
	if (!mappingAddress)
	{
		throw std::exception("Unable to retrieve mapping address.");
	}

	return static_cast<std::byte*>(mappingAddress);
}
