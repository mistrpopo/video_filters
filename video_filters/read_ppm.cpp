#include "read_ppm.h"
#include <fstream>
#include <iostream>

using std::ifstream;
//small ugly convenient macro
#define CERROR(message) std::cerr << __FUNCTION__ << ": " << message << std::endl

bool read_ppm_pbm(ifstream& in, rgb_image& result, bool ascii);
bool read_ppm_pgm(ifstream& in, rgb_image& result, bool ascii);
bool read_ppm_ppm(ifstream& in, rgb_image& result, bool ascii);


bool read_ppm(const std::string & filename, rgb_image& result)
{
	ifstream in(filename);
	if (!in.is_open())
	{
		CERROR("Could not open file " << filename);
		return false;
	}
	
	std::string ppm_version;
	in >> ppm_version;
	in >> std::ws; //eat up remaining white-space
	while (in.peek() == '#') //eat up comment lines
	{
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		in >> std::ws;
	}
	in >> result.width >> result.height;
	in >> std::ws; //eat up remaining white-space
	result.allocate();

	bool is_binary =
		ppm_version == "P4" || ppm_version == "P5" || ppm_version == "P6";
	if (is_binary)
	{
		//re-open the file in binary mode
		std::streampos data_offset = in.tellg();
		in = ifstream(filename, std::ios::in | std::ios::binary);
		
		in.seekg(data_offset);
	}

	if (ppm_version == "P1" || ppm_version == "P4")
	{
		return read_ppm_pbm(in, result, !is_binary);
	}
	else if (ppm_version == "P2" || ppm_version == "P5")
	{
		return read_ppm_pgm(in, result, !is_binary);
	}
	else if (ppm_version == "P3" || ppm_version == "P6")
	{
		return read_ppm_ppm(in, result, !is_binary);
	}
	CERROR("Unrecognized version " << ppm_version);
	return false;
}

bool read_ppm_pbm(ifstream& in, rgb_image& result, bool ascii)
{
	int pixel = 0;
	if (ascii)
	{
		int32_t in_value = 0;
		while (in >> in_value)
		{
			//the explicit constructor is to help msvc with C++ features less than 10 y/o
			result(pixel) = (in_value == 0) ? rgb_pixel{ 255, 255, 255 } : rgb_pixel{ 0, 0, 0 };
			++pixel;
		}
	}
	else
	{
		uint8_t in_byte = 0;
		while (in.read((char*)&in_byte, 1))
		{
			for (int i = 0; i < 8; ++i)
			{
				result(pixel) = ((in_byte & (1 << i)) == 0) ? rgb_pixel{ 255, 255, 255 } : rgb_pixel{ 0, 0, 0 };
				++pixel;
				if (pixel % result.width == 0) break;
			}
		}
	}
	return true;
}

bool read_ppm_pgm(ifstream& in, rgb_image& result, bool ascii)
{
	int32_t in_value = 0;
	int pixel = 0;
	if (ascii)
	{
		while (in >> in_value)
		{
			result(pixel) = { in_value, in_value, in_value };
			++pixel;
		}
	}
	else
	{
		uint8_t in_byte = 0;
		while (in.read((char*)&in_byte, 1))
		{
			result(pixel) = { in_value, in_value, in_value };
			++pixel;
		}
	}
	return true;
}

bool read_ppm_ppm(ifstream& in, rgb_image& result, bool ascii)
{
	int32_t in_value_r = 0, in_value_g = 0, in_value_b = 0;
	int pixel = 0;
	if (ascii)
	{
		while (in >> in_value_r >> in_value_g >> in_value_b)
		{
			result(pixel) = { in_value_r, in_value_g, in_value_b };
			++pixel;
		}
	}
	else
	{
		uint8_t in_bytes[3];
		while (in.read((char*)in_bytes, 3))
		{
			result(pixel) = { in_bytes[0], in_bytes[1], in_bytes[2] };
			++pixel;
		}
	}
	return true;
}
