#pragma once

#ifdef LIBPIC_EXPORTS
#define LIBPIC_API _declspec(dllexport)
#else
#define LIBPIC_API _declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

extern "C" LIBPIC_API typedef unsigned char image_byte;
extern "C" LIBPIC_API typedef image_byte* image_buffer;
extern "C" LIBPIC_API typedef unsigned int image_height;
extern "C" LIBPIC_API typedef unsigned int image_width;
extern "C" LIBPIC_API typedef unsigned int image_dimension;
extern "C" LIBPIC_API typedef unsigned char image_color_type;
extern "C" LIBPIC_API typedef unsigned char image_bit_depth;

extern "C" struct LIBPIC_API image
{
	image_width width;
	image_height height;

	image_color_type color_type;
	image_bit_depth bit_depth;

	image_buffer buffer;
};

extern "C" LIBPIC_API image load_image(const char* filename);

#undef WIN32_LEAN_AND_MEAN
