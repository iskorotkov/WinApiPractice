#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "filestream.h"
#include <fstream>
#include "preferences.h"
#include <sstream>
#include <codecvt>


Preferences* ReadConfigUsingStream(const TCHAR* configFile)
{
	std::wifstream ifs(configFile);

	// TODO: std::little_endian is deprecated.
	ifs.imbue(std::locale(ifs.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>));

	std::wostringstream oss;
	oss << ifs.rdbuf();

	return StringToPreferences(oss.str().c_str());
}

void WriteConfigUsingStream(const TCHAR* configFile, const Preferences* prefs)
{
	const TCHAR* content = PreferencesToString(prefs);

	std::wofstream ofs;
	ofs << content;

	delete content;
}

#undef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
