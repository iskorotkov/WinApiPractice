#pragma once

#ifdef LIBPIC_EXPORTS
#define LIBPIC_API _declspec(dllexport)
#else
#define LIBPIC_API _declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN

extern "C" LIBPIC_API using image_byte = unsigned char;
extern "C" LIBPIC_API using image_buffer = image_byte*;
extern "C" LIBPIC_API using image_height = unsigned int;
extern "C" LIBPIC_API using image_width = unsigned int;
extern "C" LIBPIC_API using image_dimension = unsigned int;
extern "C" LIBPIC_API using image_color_type = unsigned char;
extern "C" LIBPIC_API using image_bit_depth = unsigned char;

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
