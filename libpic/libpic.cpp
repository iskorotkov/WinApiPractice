#include "pch.h"
#include "libpic.h"
#include <filesystem>
#include "PngWrapper.h"
#include "JpegWrapper.h"

Image LoadPic(const char* filename)
{
	using std::filesystem::path;

	const path p(filename);
	const auto extension = p.extension();
	if (extension == path(".png"))
	{
		return ReadPngFile(filename);
	}
	if (extension == path(".jpeg")
		|| extension == path(".jpg")
		|| extension == path(".jpe")
		|| extension == path(".jfif"))
	{
		return ReadJpegImage(filename);
	}
	// TODO: Add error handling.
	return Image();
}

bool IsValid(Image& img)
{
	return CalcBufferSize(img) > 0;
}

int CalcBufferSize(Image& img)
{
	return CalcRowSize(img) * img.Height;
}

int CalcRowSize(Image& img)
{
	const auto bitsInByte = 8;
	return img.BitDepth * img.Width / bitsInByte;
}
