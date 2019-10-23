#pragma once
#include <cstddef>

// ReSharper disable once CppInconsistentNaming
namespace FromRGB
{
	// ReSharper disable once CppInconsistentNaming
	void ToRGBA(const std::byte* source, std::byte* dest);
	
	// ReSharper disable once CppInconsistentNaming
	void ToRGB(const std::byte* source, std::byte* dest);
};

namespace Change
{
	void ChannelsNumber(const std::byte* source, std::byte* dest, int sourceChannels, int destChannels);
}
