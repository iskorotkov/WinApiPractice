#pragma once
#include "Windows.h"
#include <string>

class LibraryHandle
{
public:
	explicit LibraryHandle(const wchar_t* name);
	explicit LibraryHandle(const std::wstring& name);

	[[nodiscard]] FARPROC GetMethod(const wchar_t* name) const;
	[[nodiscard]] FARPROC GetMethod(const std::wstring& name) const;

	[[nodiscard]] bool IsValid() const;

	LibraryHandle(const LibraryHandle& other) = delete;
	LibraryHandle(LibraryHandle&& other) noexcept = delete;
	LibraryHandle& operator=(const LibraryHandle& other) = delete;
	LibraryHandle& operator=(LibraryHandle&& other) noexcept = delete;

	~LibraryHandle();

private:
	HMODULE library;
};
