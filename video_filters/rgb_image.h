#pragma once
#include <stdint.h>

#define FALL_IN_SINK 1
#define THROW 2
#define UNCHECKED 3

#define INVALID_ACCESS_POLICY THROW





struct rgb_pixel
{
	int32_t r;
	int32_t g;
	int32_t b;
};

struct rgb_line
{
	rgb_line(int width, rgb_pixel* data);
	rgb_pixel& operator[](int j);

	int width;
	rgb_pixel* data;
};

struct rgb_image
{
	rgb_image();
	rgb_image(int w, int h, bool allocate = true);
	~rgb_image();
	
	void allocate();

	rgb_line operator[](int i);
	rgb_pixel& operator()(int pixel);

	int width;
	int height;

	rgb_pixel* data;

};

