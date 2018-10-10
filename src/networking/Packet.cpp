#include "Packet.hpp"

Packet::Packet(PacketHeader p_header)
	: header(p_header)
	, bytes(sizeof(PacketHeader))
	, bytes_written(sizeof(PacketHeader))
	, bytes_read(sizeof(PacketHeader))
	, bits_written(0)
	, bits_read(0)
{
	bytes.reserve(1400);
	memcpy(bytes.data(), &header, sizeof(PacketHeader));
}

void Packet::write_bits(int& data, int bits_to_write, int offset)
{
	//pseudocode
	/*
		if bits_written = 8, add 1 to bytes_written

		for each bit starting from the offset amount
			if the bit is 1
				set the other bit to 1 after considering negative offset
			otherwise set the other bit to 0

		increment bits_written
	*/
}

void Packet::read_bits(int& data, int bits_to_write, int offset)
{
	//pseudocode
	/*
	*/
}

void Packet::set_header(PacketHeader p_header)
{
	header = p_header;
	memcpy(bytes.data(), &header, sizeof(PacketHeader));
}

const PacketHeader& Packet::get_header() const
{
	return header;
}

const std::vector<std::byte>& Packet::get_bytes() const
{
	return bytes;
}

Packet& Packet::operator <<(std::string data)
{
	*this << data.length();
	serialize(data.data(), data.length());
	return *this;
}

Packet& Packet::operator >>(std::string& data)
{
	std::size_t length;
	*this >> length;
	data.resize(length);
	deserialize(data.data(), length);
	return *this;
}