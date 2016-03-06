#pragma once
#include <string>
#include "rgb_image.h"

//read single image in pbm, pgm or ppm format
bool read_ppm(const std::string& filename, rgb_image& result);