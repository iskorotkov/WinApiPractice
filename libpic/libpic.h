#pragma once

#ifdef LIBPIC_EXPORTS
#define LIBPIC_API _declspec(dllexport)
#else
#define LIBPIC_API _declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

extern "C" LIBPIC_API typedef unsigned char ImageByte;
extern "C" LIBPIC_API typedef ImageByte* ImageBuffer;
extern "C" LIBPIC_API typedef unsigned int ImageHeight;
extern "C" LIBPIC_API typedef unsigned int ImageWidth;
extern "C" LIBPIC_API typedef unsigned int ImageDimension;
extern "C" LIBPIC_API typedef unsigned char ImageColorType;
extern "C" LIBPIC_API typedef unsigned char ImageBitDepth;

extern "C" struct LIBPIC_API Image
{
	ImageWidth Width;
	ImageHeight Height;

	ImageColorType ColorType;
	ImageBitDepth BitDepth;

	ImageBuffer Buffer;
};

extern "C" LIBPIC_API Image LoadPic(const char* filename);

#undef WIN32_LEAN_AND_MEAN
