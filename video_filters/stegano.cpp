#include "stegano.h"
//#include <bitset> //that's crap
#include <algorithm>
#include <iostream>
//small ugly convenient macro
#define CERROR(message) std::cerr << __FUNCTION__ << ": " << message << std::endl

//maximum allowed SNR (number of bits of signal for one byte of data. the lower the better. possible values 1, 2, 3, 4)
static const int MAX_SNR = 4;
//header size: how many bytes will be used to store the parameters
static const int STEGA_HEADER_SIZE = 1 + 4;
//hardcoded SNR for the header of 2 bits/byte
static const int STEGA_HEADER_SNR = 2;
//header 'magic' (can be used for position calibration, or version upgrade)
static const char HEADER_MAGIC = '\xee';

std::string get_message_header(const uint32_t &message_size);
bool write_string(const uint8_t* source_data, const std::string & string, uint8_t*& target_data, const int snr);

inline size_t encoded_size(const size_t size, const int snr)
{
	return static_cast<size_t>(ceil(static_cast<double>(size * 8) / snr));
}

inline uint8_t bit_mask(const size_t how_many)
{
	return ((1 << how_many) - 1);
}

bool steganography_encode(const rgb_image & source, const std::string & message, rgb_image & target)
{
	const size_t carrier_data_size = source.size() * 3;
	if (message.size() > UINT32_MAX || encoded_size(STEGA_HEADER_SIZE, STEGA_HEADER_SNR) + encoded_size(message.size(), MAX_SNR) > carrier_data_size)
	{
		CERROR("Excessive message size " << message.size());
		return false;
	}
	if (!source)
	{
		CERROR("Source image invalid");
		return false;
	}
	const uint32_t message_size = static_cast<uint32_t>(message.size());
	target = rgb_image(source.width, source.height);
	std::string header = get_message_header(message_size);

	const uint8_t* source_data = reinterpret_cast<const uint8_t*>(source.data);
	uint8_t* target_data = reinterpret_cast<uint8_t*>(target.data);
	if (!write_string(source_data, header, target_data, STEGA_HEADER_SNR)) return false;
	//todo calculate the snr as function of message size and image size 
	//the same calculation should be done at decode phase (message size can be retrieved in decode because the header has fixed parameters)
	int snr = STEGA_HEADER_SNR;
	if (!write_string(source_data, message, target_data, snr)) return false;
	return true;
}

std::string get_message_header(const uint32_t &message_size)
{
	std::string header(STEGA_HEADER_SIZE,'\0');
	header[0] = HEADER_MAGIC;
	header[1] = (message_size >> 24) & 0xFF;
	header[2] = (message_size >> 16) & 0xFF;
	header[3] = (message_size >> 8) & 0xFF;
	header[4] = (message_size >> 0) & 0xFF;
	return header;
}

bool write_string(const uint8_t* source_data, const std::string & string, uint8_t*& target_data, const int snr)
{
	size_t all_bytes = encoded_size(string.size(), snr);
	size_t total_bits_encoded = 0;

	for (size_t i = 0; i < all_bytes; ++i)
	{
		uint8_t target_byte = source_data[i] & ~bit_mask(snr);
		size_t bits_to_encode = snr;
		size_t bits_encoded = 0;
		while (bits_to_encode)
		{
			const size_t string_byte_position = total_bits_encoded / 8;
			const size_t string_bit_position  = total_bits_encoded % 8;
			uint8_t string_data = string[string_byte_position];
			const size_t bits_from_this_position = std::min<size_t>(8 - string_bit_position, snr - bits_encoded);

			//todo i did all this mess to make encoding work with 3 bits-per-byte, but there's still something wrong here 
			//need to zero out all unrelated bits. 
			//e.g. string_data has 2 bits left, but taget_byte has 3 => need to shift one-up, and make a bitmask like 0000 0110 to keep only 2bytes
			string_data >>= (8 - string_bit_position - bits_from_this_position);
			string_data &= bit_mask(bits_from_this_position);
			target_byte |= string_data;

			bits_to_encode -= bits_from_this_position;
			bits_encoded += bits_from_this_position;
			total_bits_encoded += bits_from_this_position;
		}
		(*target_data++) = target_byte;
	}
	return true;
}

bool steganography_decode(const rgb_image & source, std::string & message)
{
	return false;
}
