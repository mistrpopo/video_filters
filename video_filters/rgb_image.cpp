#include "rgb_image.h"
#include <stdlib.h>
#if INVALID_ACCESS_POLICY == THROW
#include <exception>
#endif

#if INVALID_ACCESS_POLICY == FALL_IN_SINK
//all invalid pixel access operations will fall into this
static rgb_pixel out_of_bounds;
#endif
	
rgb_image::rgb_image()
	:width(0), height(0), data(nullptr)
{

}

rgb_image::rgb_image(int w, int h, bool allocate_in_ctor /*= true*/)
	:width(w), height(h), data(nullptr)
{
	if (allocate_in_ctor) allocate();
}


rgb_image::~rgb_image()
{
	if (data) free(data);
}

rgb_line::rgb_line(int w, rgb_pixel* d)
	:width(w), data(d)
{
}

void rgb_image::allocate()
{
	if (!width || !height) return;
	data = static_cast<rgb_pixel*>(calloc(width * height, sizeof(rgb_pixel)));
}

rgb_line rgb_image::operator[](int i)
{
#if INVALID_ACCESS_POLICY == FALL_IN_SINK
	return rgb_line(width,data && i >= 0 && i < height ? &data[i * width] : nullptr);
#elif INVALID_ACCESS_POLICY == THROW
	if(data && i >= 0 && i < height)
		return rgb_line(width, &data[i * width]);
	throw std::exception("invalid access");
#endif
}

rgb_pixel& rgb_image::operator()(int pixel)
{
#if INVALID_ACCESS_POLICY == FALL_IN_SINK
	return reinterpret_cast<rgb_pixel&>(data && pixel >= 0 && pixel < width * height ? data[pixel] : out_of_bounds);
#elif INVALID_ACCESS_POLICY == THROW
	if (data && pixel >= 0 && pixel < width * height)
		return reinterpret_cast<rgb_pixel&>(data[pixel]);
	throw std::exception("invalid access");
#endif
}

rgb_pixel& rgb_line::operator[](int j)
{
#if INVALID_ACCESS_POLICY == FALL_IN_SINK
	return reinterpret_cast<rgb_pixel&>(data && j >= 0 && j < width ? data[j] : out_of_bounds);
#elif INVALID_ACCESS_POLICY == THROW
	if (data && j >= 0 && j < width)
		return reinterpret_cast<rgb_pixel&>(data[j]);
	throw std::exception("invalid access");
#endif
}
