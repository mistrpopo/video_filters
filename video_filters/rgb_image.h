#pragma once
#include <stdint.h>

// Disclaimer: This is my little C++ playground
// If I were you, I would probably not use rgb_image for practical purposes
 

enum INVALID_ACCESS_POLICY
{
	FALL_IN_SINK = 1,
	THROW = 2,
	UNCHECKED = 3,
	DEFAULT = FALL_IN_SINK
};

struct rgb_pixel
{
	int32_t r;
	int32_t g;
	int32_t b;
};

template<INVALID_ACCESS_POLICY policy>
struct rgb_line_
{
	rgb_line_();
	rgb_line_(int width, rgb_pixel* data);

	const rgb_pixel& operator[](int j) const;
	rgb_pixel& operator[](int j);

	int width;
	rgb_pixel* data;
};

template<INVALID_ACCESS_POLICY policy>
struct rgb_image_
{
	rgb_image_();
	rgb_image_(int w, int h, bool allocate = true);
	//transfer ownership only. no copy constructor, no shared ownership.
	rgb_image_(rgb_image_&& other);
	rgb_image_& operator=(rgb_image_&& other);
	rgb_image_(const rgb_image_& other) = delete;
	rgb_image_& operator=(const rgb_image_& other) = delete;
	~rgb_image_();
	
	void allocate();

	const rgb_line_<policy> operator[](int i) const;
	rgb_line_<policy> operator[](int i);

	const rgb_pixel& operator()(int pixel) const;
	rgb_pixel& operator()(int pixel);

	size_t size() const;

	operator bool() const;

	int width;
	int height;

	rgb_pixel* data;

};


template<INVALID_ACCESS_POLICY policy>
rgb_image_<policy>::rgb_image_()
	:width(0), height(0), data(nullptr)
{

}

template<INVALID_ACCESS_POLICY policy>
rgb_image_<policy>::rgb_image_(int w, int h, bool allocate_in_ctor /*= true*/)
	:width(w), height(h), data(nullptr)
{
	if (allocate_in_ctor) allocate();
}

template<INVALID_ACCESS_POLICY policy>
inline rgb_image_<policy>::rgb_image_(rgb_image_ && other)
	:width(other.width), height(other.height), data(other.data)
{
	other.data = nullptr;
}

template<INVALID_ACCESS_POLICY policy>
inline rgb_image_<policy>& rgb_image_<policy>::operator=(rgb_image_ && other)
{
	width = other.width;
	height = other.height;
	data = other.data;
	other.data = nullptr;
	return *this;
}

template<INVALID_ACCESS_POLICY policy>
rgb_image_<policy>::~rgb_image_()
{
	if (data) free(data);
}

template<INVALID_ACCESS_POLICY policy>
rgb_line_<policy>::rgb_line_()
	:width(0), data(nullptr)
{
}

template<INVALID_ACCESS_POLICY policy>
rgb_line_<policy>::rgb_line_(int w, rgb_pixel* d)
	:width(w), data(d)
{
}

template<INVALID_ACCESS_POLICY policy>
void rgb_image_<policy>::allocate()
{
	if (!width || !height) return;
	data = static_cast<rgb_pixel*>(calloc(width * height, sizeof(rgb_pixel)));
}

template<INVALID_ACCESS_POLICY policy>
rgb_image_<policy>::operator bool() const
{
	return (data && width != 0 && height != 0);
}

template<INVALID_ACCESS_POLICY policy>
rgb_pixel& rgb_line_<policy>::operator[](int j)
{
	//thank you C++!
	return const_cast<rgb_pixel&>(const_cast<const rgb_line_<policy>&>(*this)[j]);
}

template<INVALID_ACCESS_POLICY policy>
rgb_line_<policy> rgb_image_<policy>::operator[](int i)
{
	//thank you C++!
	return const_cast<rgb_line_<policy>&>(const_cast<const rgb_image_<policy>&>(*this)[i]);
}

template<INVALID_ACCESS_POLICY policy>
rgb_pixel& rgb_image_<policy>::operator()(int j)
{
	//thank you C++!
	return const_cast<rgb_pixel&>(const_cast<const rgb_image_<policy>&>(*this)(j));
}

template<INVALID_ACCESS_POLICY policy>
inline size_t rgb_image_<policy>::size() const
{
	return width*height;
}


using rgb_line = rgb_line_<DEFAULT>;
using rgb_image = rgb_image_<DEFAULT>;