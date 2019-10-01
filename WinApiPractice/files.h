#pragma once

enum class ReadingMethod
{
	Unknown,
	FileMapping,
	Variables,
	Streams,
	WinApi,
};

void WriteConfigFile(ReadingMethod method, struct Preferences* prefs);

Preferences* ReadConfigFile(ReadingMethod method);
