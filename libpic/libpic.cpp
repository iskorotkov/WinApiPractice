#include "pch.h"
#include "libpic.h"
#include <cstdlib>
#include <cstdio>

// Libs
#include "png.h"
#include "jpeglib.h"

image read_png_file(const char* filename)
{
	FILE* fp;
	fopen_s(&fp, filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) abort();

	png_infop info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	image img{};
	img.width = png_get_image_width(png, info);
	img.height = png_get_image_height(png, info);
	img.color_type = png_get_color_type(png, info);
	img.bit_depth = png_get_bit_depth(png, info) * png_get_channels(png, info);;

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (img.bit_depth == 16)
		png_set_strip_16(png);

	if (img.bit_depth == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (img.color_type == PNG_COLOR_TYPE_GRAY && img.bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (img.color_type == PNG_COLOR_TYPE_RGB ||
		img.color_type == PNG_COLOR_TYPE_GRAY ||
		img.color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (img.color_type == PNG_COLOR_TYPE_GRAY ||
		img.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	//row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * *height);
	//for (int y = 0; y < *height; y++)
	//{
	//	row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
	//}

	// ======================

	const auto size = sizeof(png_byte) * img.height * png_get_rowbytes(png, info);
	img.buffer = new image_byte[size];

	wprintf_s(L"Required buffer size: %d\n", sizeof(png_byte) * img.height * png_get_rowbytes(png, info));
	wprintf_s(L"Buffer size for allocation: %d\n", size);

	png_bytep* row_pointers = new png_bytep[img.height];
	for (auto i = 0u; i < img.height; ++i)
	{
		row_pointers[i] = img.buffer + png_get_rowbytes(png, info) * i;
	}
	// ======================

	png_read_image(png, row_pointers);

	fclose(fp);

	png_destroy_read_struct(&png, &info, NULL);

	// ====================
	// Swap R and B values.
	for (auto i = 0u; i < img.height * img.width * 4; i += 4)
	{
		auto swap = [](auto a, auto b) { auto c = a; a = b; b = c; };
		swap(img.buffer[i + 0], img.buffer[i + 2]);
	}
	// ====================
	return img;
}

image load_image(const char* filename)
{
	return read_png_file(filename);
}
