#include "pch.h"
#include "libpic.h"
#include <cstdlib>
#include <cstdio>

// Libs
#include "png.h"
#include "jpeglib.h"

Image ReadPngFile(const char* filename)
{
	FILE* fp;
	fopen_s(&fp, filename, "rb");

	auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png) abort();

	auto info = png_create_info_struct(png);
	if (!info) abort();

	if (setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	Image img{};
	img.Width = png_get_image_width(png, info);
	img.Height = png_get_image_height(png, info);
	img.ColorType = png_get_color_type(png, info);
	img.BitDepth = png_get_bit_depth(png, info) * png_get_channels(png, info);;

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if (img.BitDepth == 16)
		png_set_strip_16(png);

	if (img.BitDepth == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if (img.ColorType == PNG_COLOR_TYPE_GRAY && img.BitDepth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if (img.ColorType == PNG_COLOR_TYPE_RGB ||
		img.ColorType == PNG_COLOR_TYPE_GRAY ||
		img.ColorType == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if (img.ColorType == PNG_COLOR_TYPE_GRAY ||
		img.ColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	// Create array for image bytes
	const auto size = sizeof(png_byte) * img.Height * png_get_rowbytes(png, info);
	img.Buffer = new ImageByte[size];
	
	const auto rowPointers = new png_bytep[img.Height];
	for (auto i = 0u; i < img.Height; ++i)
	{
		rowPointers[i] = img.Buffer + png_get_rowbytes(png, info) * i;
	}

	png_read_image(png, rowPointers);

	fclose(fp);

	png_destroy_read_struct(&png, &info, nullptr);

	// Swap R and B values.
	for (auto i = 0u; i < img.Height * img.Width * 4; i += 4)
	{
		auto swap = [](auto& a, auto& b) { auto c = a; a = b; b = c; };
		swap(img.Buffer[i + 0], img.Buffer[i + 2]);
	}
	
	return img;
}

Image LoadPic(const char* filename)
{
	return ReadPngFile(filename);
}
