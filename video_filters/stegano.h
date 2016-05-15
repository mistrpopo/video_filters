#pragma once
#include <string>
#include "rgb_image.h"

bool steganography_encode(const rgb_image& source, const std::string& message, rgb_image& target);

bool steganography_decode(const rgb_image& source, std::string& message);