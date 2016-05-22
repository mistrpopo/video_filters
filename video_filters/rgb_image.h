#pragma once
#include <stdint.h>
#include <iomanip>
#include <exception>

// Disclaimer: This is my little C++ playground
// If I were you, I would probably not use rgb_image for practical purposes

enum INVALID_ACCESS_POLICY
{
	FALL_IN_SINK = 1,
	THROW = 2,
	UNCHECKED = 3,
	DEFAULT = FALL_IN_SINK
};

typedef uint8_t pixel_t;

template<typename pixel_type>
struct rgb_pixel_
{
	pixel_type r;
	pixel_type g;
	pixel_type b;
};

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
struct rgb_line_
{
	rgb_line_();
	rgb_line_(int width, rgb_pixel_<pixel_type>* data);

	const rgb_pixel_<pixel_type>& operator[](int j) const;
	rgb_pixel_<pixel_type>& operator[](int j);

	int width;
	rgb_pixel_<pixel_type>* data;
};

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
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

	const rgb_line_<pixel_type, policy> operator[](int i) const;
	rgb_line_<pixel_type, policy> operator[](int i);

	const rgb_pixel_<pixel_type>& operator()(int pixel) const;
	rgb_pixel_<pixel_type>& operator()(int pixel);

	size_t size() const;

	operator bool() const;

	int width;
	int height;

	rgb_pixel_<pixel_type>* data;

};


//all "fall-in-sink" invalid pixel access operations will end up writing in this pixel
template<typename pixel_type>
static rgb_pixel_<pixel_type> out_of_bounds;


template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline const rgb_pixel_<pixel_type>& rgb_line_<pixel_type, policy>::operator[](int j) const
{
	switch (policy)
	{
	case FALL_IN_SINK:
		if (data && j >= 0 && j < width) { return data[j]; }
		return out_of_bounds<pixel_type>;
	case THROW:
		if (data && j >= 0 && j < width) { return data[j]; }
		throw std::exception("invalid access");
	case UNCHECKED:
	default:
		return data[j];
	}
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline const rgb_line_<pixel_type, policy> rgb_image_<pixel_type, policy>::operator[](int i) const
{
	switch (policy)
	{
	case FALL_IN_SINK:
		if (data && i >= 0 && i < height) { return rgb_line_<pixel_type, policy>(width, &data[i * width]); }
		return rgb_line_<pixel_type, FALL_IN_SINK>();
	case THROW:
		if (data && i >= 0 && i < height) { return rgb_line_<pixel_type, policy>(width, &data[i * width]); }
		throw std::exception("invalid access");
	case UNCHECKED:
	default:
		return rgb_line_<pixel_type, policy>(width, &data[i * width]);
	}
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline const rgb_pixel_<pixel_type>& rgb_image_<pixel_type, policy>::operator()(int pixel) const
{
	switch (policy)
	{
	case FALL_IN_SINK:
		if (data && pixel >= 0 && pixel < width * height) { return data[pixel]; }
		return out_of_bounds<pixel_type>;
	case THROW:
		if (data && pixel >= 0 && pixel < width * height) { return data[pixel]; }
		throw std::exception("invalid access");
	case UNCHECKED:
	default:
		return data[pixel];
	}
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_image_<pixel_type, policy>::rgb_image_()
	:width(0), height(0), data(nullptr)
{

}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_image_<pixel_type, policy>::rgb_image_(int w, int h, bool allocate_in_ctor /*= true*/)
	:width(w), height(h), data(nullptr)
{
	if (allocate_in_ctor) allocate();
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline rgb_image_<pixel_type, policy>::rgb_image_(rgb_image_ && other)
	:width(other.width), height(other.height), data(other.data)
{
	other.data = nullptr;
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline rgb_image_<pixel_type, policy>& rgb_image_<pixel_type, policy>::operator=(rgb_image_ && other)
{
	width = other.width;
	height = other.height;
	data = other.data;
	other.data = nullptr;
	return *this;
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_image_<pixel_type, policy>::~rgb_image_()
{
	if (data) free(data);
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_line_<pixel_type, policy>::rgb_line_()
	:width(0), data(nullptr)
{
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_line_<pixel_type, policy>::rgb_line_(int w, rgb_pixel_<pixel_type>* d)
	:width(w), data(d)
{
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
void rgb_image_<pixel_type, policy>::allocate()
{
	if (!width || !height) return;
	data = static_cast<rgb_pixel_<pixel_type>*>(calloc(width * height, sizeof(rgb_pixel_<pixel_type>)));
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_image_<pixel_type, policy>::operator bool() const
{
	return (data && width != 0 && height != 0);
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_pixel_<pixel_type>& rgb_line_<pixel_type, policy>::operator[](int j)
{
	//thank you C++!
	return const_cast<rgb_pixel_<pixel_type>&>(const_cast<const rgb_line_<pixel_type, policy>&>(*this)[j]);
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_line_<pixel_type, policy> rgb_image_<pixel_type, policy>::operator[](int i)
{
	//thank you C++!
	return const_cast<rgb_line_<pixel_type, policy>&>(const_cast<const rgb_image_<pixel_type, policy>&>(*this)[i]);
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
rgb_pixel_<pixel_type>& rgb_image_<pixel_type, policy>::operator()(int j)
{
	//thank you C++!
	return const_cast<rgb_pixel_<pixel_type>&>(const_cast<const rgb_image_<pixel_type, policy>&>(*this)(j));
}

template<typename pixel_type, INVALID_ACCESS_POLICY policy>
inline size_t rgb_image_<pixel_type, policy>::size() const
{
	return width*height;
}

//some iostream b***s***. please kill me. who wants to write code like that??
namespace std
{
	template<typename pixel_type>
	ostream& operator<<(ostream& os, const rgb_pixel_<pixel_type>& p)
	{
		ios::fmtflags f(os.flags());
		os.flags(std::ios::hex);
		os << '[' 
			<< std::setfill('0') << std::setw(2) << static_cast<int>(p.r) << ',' 
			<< std::setfill('0') << std::setw(2) << static_cast<int>(p.g) << ','
			<< std::setfill('0') << std::setw(2) << static_cast<int>(p.b) << ']';
		os.flags(f);
		return os;
	}
}


using rgb_pixel = rgb_pixel_<pixel_t>;
using rgb_line  = rgb_line_ <pixel_t, DEFAULT>;
using rgb_image = rgb_image_<pixel_t, DEFAULT>;
