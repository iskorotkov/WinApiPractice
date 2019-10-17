#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

using image_byte = unsigned char;
using image_buffer = image_byte*;
using image_height = unsigned int;
using image_width = unsigned int;
using image_dimension = unsigned int;
using image_color_type = unsigned char;
using image_bit_depth = unsigned char;

struct image
{
	image_width width;
	image_height height;

	image_color_type color_type;
	image_bit_depth bit_depth;
	
	image_buffer buffer;
};

extern "C" __declspec(dllexport) image load_image(const char* filename);
