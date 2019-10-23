#include "LibraryHandle.h"

LibraryHandle::LibraryHandle(const wchar_t* name)
{
	library = LoadLibrary(name);
	if (!IsValid())
	{
		throw std::exception("Attempt to access DLL function while DLL is not loaded.");
	}
}

LibraryHandle::LibraryHandle(const std::wstring& name) : LibraryHandle(name.c_str())
{
}

FARPROC LibraryHandle::GetMethod(const char* name) const
{
	const auto address = GetProcAddress(library, name);
	if (!address)
	{
		throw std::exception("Unable to find a function.");
	}
	return address;
}

FARPROC LibraryHandle::GetMethod(const std::string& name) const
{
	return GetMethod(name.c_str());
}

bool LibraryHandle::IsValid() const
{
	return library != nullptr;
}

LibraryHandle::~LibraryHandle()
{
	if (IsValid())
	{
		FreeLibrary(library);
	}
}
