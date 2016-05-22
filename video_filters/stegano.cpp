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

bool write_string(const uint8_t* source_data, const std::string & string, uint8_t*& target_data, const int snr);
bool read_string(const uint8_t*& source_data, std::string & string, const int snr, const uint32_t size);

std::string get_message_header(const uint32_t &message_size);
uint32_t get_message_size(const std::string &message_header);

inline int get_min_snr(const size_t carrier_size, const uint32_t message_size) { return 3; } //TODO
inline size_t encoded_size(const size_t size, const int snr) { return static_cast<size_t>(ceil(static_cast<double>(size * 8) / snr)); }
inline uint8_t bit_mask(const size_t how_many) { return ((1 << how_many) - 1); }

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
	int snr = get_min_snr(source.size(), message_size);
	if (!write_string(source_data, message, target_data, snr)) return false;
	return true;
}

bool steganography_decode(const rgb_image & source, std::string & message)
{
	const size_t carrier_data_size = source.size() * 3;
	const uint8_t* source_data = reinterpret_cast<const uint8_t*>(source.data);
	std::string header;
	if (!read_string(source_data, header, STEGA_HEADER_SNR, STEGA_HEADER_SIZE)) return false;
	uint32_t message_size = get_message_size(header);
	int snr = get_min_snr(source.size(), message_size);
	if (!read_string(source_data, message, snr, message_size)) return false;
	return true;
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
			const size_t bits_from_this_position = std::min<size_t>(8 - string_bit_position, snr - bits_encoded);

			char string_data = string[string_byte_position];
			string_data >>= (8 - string_bit_position - bits_from_this_position);
			string_data &= bit_mask(bits_from_this_position);
			string_data <<= (snr - bits_from_this_position - bits_encoded);
			target_byte |= string_data;

			bits_to_encode -= bits_from_this_position;
			bits_encoded += bits_from_this_position;
			total_bits_encoded += bits_from_this_position;
		}
		target_data[i] = target_byte;
	}
	target_data += all_bytes;
	return true;
}

bool read_string(const uint8_t*& source_data, std::string & string, const int snr, const uint32_t size)
{
	string.resize(size,'\0');
	size_t all_bytes = encoded_size(size, snr);
	size_t total_bits_decoded = 0;

	for (size_t i = 0; i < size; ++i)
	{
		char target_char = '\0';
		size_t bits_to_decode = 8;
		size_t bits_decoded = 0;
		while (bits_to_decode)
		{
			const size_t buffer_byte_position = total_bits_decoded / snr;
			const size_t buffer_bit_position  = total_bits_decoded % snr;
			const size_t bits_from_this_position = std::min<size_t>(snr - buffer_bit_position, 8 - bits_decoded);

			uint8_t buffer_data = source_data[buffer_byte_position];
			buffer_data >>= (snr - buffer_bit_position - bits_from_this_position);
			buffer_data &= bit_mask(bits_from_this_position);
			buffer_data <<= (8 - bits_from_this_position - bits_decoded);
			target_char |= buffer_data;

			bits_to_decode -= bits_from_this_position;
			bits_decoded += bits_from_this_position;
			total_bits_decoded += bits_from_this_position;
		}
		string[i] = target_char;
	}
	source_data += all_bytes;
	return true;
}

std::string get_message_header(const uint32_t &message_size)
{
	std::string header(STEGA_HEADER_SIZE, '\0');
	header[0] = HEADER_MAGIC;
	header[1] = (message_size >> 24) & 0xFF;
	header[2] = (message_size >> 16) & 0xFF;
	header[3] = (message_size >> 8 ) & 0xFF;
	header[4] = (message_size >> 0 ) & 0xFF;
	return header;
}

uint32_t get_message_size(const std::string & message_header)
{
	if (message_header[0] != HEADER_MAGIC)
	{
		CERROR("Wrong header magic - " << std::hex << message_header[0] << " - either the version is invalid or the data is corrupted.");
		return 0;
	}
	uint32_t size = 0;
	size |= message_header[1]<< 24;
	size |= message_header[2]<< 16;
	size |= message_header[3]<< 8 ;
	size |= message_header[4]<< 0 ;
	return size;
}

