#include "pch.h"
#include "Constants.h"

int TargetColorDepth()
{
	const auto bitsInByte = sizeof(char);
	return TargetBytesPerPixel() * bitsInByte;
}

int TargetBytesPerPixel()
{
	return 4;
}
