#include "LibraryHandle.h"

LibraryHandle::LibraryHandle(const wchar_t* name)
{
	library = LoadLibrary(name);
}

LibraryHandle::LibraryHandle(const std::wstring& name) : LibraryHandle(name.c_str())
{
}

FARPROC LibraryHandle::GetMethod(const wchar_t* name) const
{
	if (!IsValid())
	{
		throw std::exception("Attempt to access DLL function while DLL is not loaded.");
	}
	return GetProcAddress(library, "LoadPicW");
}

FARPROC LibraryHandle::GetMethod(const std::wstring& name) const
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
