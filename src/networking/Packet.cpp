#include "Packet.hpp"

Packet::Packet(PacketHeader p_header)
	: header(p_header)
	, bytes(sizeof(PacketHeader))
	, bytes_written(sizeof(PacketHeader))
	, bytes_read(sizeof(PacketHeader))
	, bits_written(8)
	, bits_read(8)
{
	bytes.reserve(1400);
	memcpy(bytes.data(), &header, sizeof(PacketHeader));
}

void Packet::write_bits(int& data, int bits_to_write, int offset)
{
	//ensure our bit/byte count is up to date
	if (bits_written >= 8)
	{
		bytes_written += bits_written / 8;
		bits_written = bits_written % 8;
	}

	int bytes_needed = ((8 - bits_written) + bits_to_write) / 8;

	if (bytes_written + bytes_needed > bytes.size())
	{
		bytes.resize(bytes_written + (bits_to_write / 8));
	}

	bytes.data()[bytes_written - 1] |= static_cast<std::byte>(data << (bits_written));

	bits_written += bits_to_write;
}

void Packet::read_bits(int& data, int bits_to_read, int offset)
{
	if (bits_read >= 8)
	{
		bytes_read += bits_read / 8;
		bits_read = bits_read % 8;
	}

	for (int i = offset; i < bits_to_read + offset; ++i)
	{
		//If this bit is 1
		int byte = static_cast<int>(bytes.data()[bytes_read - 1]);
		int shift = 1 << (bits_read + i);
		if ((byte) & (shift))
		{
			//set the other numbers bit to 1
			data |= (1 << i);
		}
		else
		{
			//otherwise set it to 0
			data &= ~(1 << i);
		}
	}

	bits_read += bits_to_read;
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