#include "pch.h"
#include "Conversions.h"
#include <exception>
#include <string>

// ReSharper disable once CppInconsistentNaming
void FromRGB::ToRGBA(const std::byte* source, std::byte* dest)
{
	dest[0] = source[2];
	dest[1] = source[1];
	dest[2] = source[0];
	dest[4] = std::byte(0);
}

// ReSharper disable once CppInconsistentNaming
void FromRGB::ToRGB(const std::byte* source, std::byte* dest)
{
	dest[0] = source[2];
	dest[1] = source[1];
	dest[2] = source[0];
}

void Change::ChannelsNumber(const std::byte* source, std::byte* dest, const int sourceChannels, const int destChannels)
{
	if (sourceChannels == 3)
	{
		if (destChannels == 4)
		{
			FromRGB::ToRGBA(source, dest);
			return;
		}
	}
	else if (sourceChannels == 4)
	{
		if (destChannels == 4)
		{
			FromRGB::ToRGB(source, dest);
			return;
		}
	}
	throw std::exception("Can't change channels number.");
}
