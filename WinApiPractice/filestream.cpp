#include "filestream.h"
#include <fstream>
#include "preferences.h"
#include <sstream>
#include <codecvt>

Preferences* ReadConfigUsingStream(const TCHAR* configFile)
{
	std::ifstream ifs(configFile, std::ios::binary);

	ifs.seekg(0, std::ios::end);
	const UINT size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	const UINT bufferSize = (size % 2 == 1 ? size + 1 : size) / sizeof(wchar_t);

	TCHAR* buffer = new TCHAR[bufferSize + 1];
	buffer[bufferSize] = L'\0';

	ifs.read((char*)buffer, size);

	return StringToPreferences(buffer);
}

void WriteConfigUsingStream(const TCHAR* configFile, const Preferences* prefs)
{
	const TCHAR* content = PreferencesToString(prefs);

	std::ofstream ofs(configFile, std::ios::binary);
	ofs.write((char*)content, wcslen(content) * sizeof(wchar_t));

	delete content;
}
