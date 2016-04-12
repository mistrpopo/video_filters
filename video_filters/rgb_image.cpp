#include "rgb_image.h"
#include <stdlib.h>
#include <exception>

//all "fall-in-sink" invalid pixel access operations will end up writing in this pixel
static rgb_pixel out_of_bounds;

template<>
const rgb_line_<FALL_IN_SINK> rgb_image_<FALL_IN_SINK>::operator[](int i) const
{
	if (data && i >= 0 && i < height) { return rgb_line_<FALL_IN_SINK>(width, &data[i * width]); }
	return rgb_line_<FALL_IN_SINK>();
}

template<>
const rgb_line_<THROW> rgb_image_<THROW>::operator[](int i) const
{
	if (data && i >= 0 && i < height) { return rgb_line_<THROW>(width, &data[i * width]); }
	throw std::exception("invalid access");
}

template<>
const rgb_line_<UNCHECKED> rgb_image_<UNCHECKED>::operator[](int i) const
{
	return rgb_line_<UNCHECKED>(width, &data[i * width]);
}

template<>
const rgb_pixel& rgb_image_<FALL_IN_SINK>::operator()(int pixel) const
{
	if (data && pixel >= 0 && pixel < width * height) { return data[pixel]; }
	return out_of_bounds;
}


template<>
const rgb_pixel& rgb_image_<THROW>::operator()(int pixel) const
{
	if (data && pixel >= 0 && pixel < width * height) { return data[pixel]; }
	throw std::exception("invalid access");
}

template<>
const rgb_pixel& rgb_image_<UNCHECKED>::operator()(int pixel) const
{
	return data[pixel];
}

template<>
const rgb_pixel& rgb_line_<FALL_IN_SINK>::operator[](int j) const
{
	if (data && j >= 0 && j < width) { return data[j]; }
	return out_of_bounds;
}

template<>
const rgb_pixel& rgb_line_<THROW>::operator[](int j) const
{
	if (data && j >= 0 && j < width) { return data[j]; }
	throw std::exception("invalid access");
}

template<>
const rgb_pixel& rgb_line_<UNCHECKED>::operator[](int j) const
{
	return data[j];
}
