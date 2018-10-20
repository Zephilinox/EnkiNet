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

	//don't support cross byte writing yet
	if (bits_to_write + bits_written > 8)
	{
		//throw std::exception();
	}

	/*
	we have 8 bits available
	we want to write 27
	27 - 8 = 19 bits we need to get
	19.f / 8.f = 2.3
	ceil(2.3) = 3
	bytes needed is 3, on top of the 1 byte we already have
	4 * 8 = 32, leaving 5 empty bits left over at the end

	okay, so now we have the required space
	how to write the bits across bytes?

	in above example, we want to write 8 bits, 8 bits, 8 bits, 3 bits
	that's 3 loops of a full byte and then the remainder
	27 / 8 = 3
	27 % 8 = 3

	bits_left = 27
	byte = 0;
	while bits_left > 0
		if (bits_left / 8 >= 1) //8 or more bits needed to write
			write 8 bits
			byte++
		else
			write bits_to_write % 8; //write remaining bits
			bits_left = 0;

	we have 2 bits available
	we want to write 4 bits
	4 - 2 = 2 bits we need to get
	2.f / 8.f = 0.25
	ceil(0.25) = 1
	bytes needed is 1, on top of the 2 bits we already have
	2 * 8 = 16, leaving (6 written + 4 to write) 6 empty bits at the end
	*/

	int bits_available = 8 - bits_written;
	int bits_needed = bits_to_write - bits_available;
	int bytes_needed = static_cast<int>(std::ceil(static_cast<float>(bits_needed) / 8.0f));

	if (bytes_written + bytes_needed > bytes.size())
	{
		bytes.resize(bytes_written + bytes_needed);
	}

	auto write_bits = [&](int bits, int extra_offset)
	{
		for (int i = 0; i < bits; ++i)
		{
			int shift = 1 << (i + offset + extra_offset);
			if (data & (shift))
			{
				bytes.data()[bytes_written - 1] |= static_cast<std::byte>((1 << (i + bits_written)));
			}
			else
			{
				bytes.data()[bytes_written - 1] &= static_cast<std::byte>(~(1 << (i + bits_written)));
			}
		}
	};

	int bits_left = bits_to_write;
	while (bits_left > 0)
	{
		if (bits_available < 8)
		{
			write_bits(bits_available, 0);
			bytes_written++;
			bits_written = 0;
			bits_left -= bits_available;
			bits_available = 8;
		}
		else if (bits_left / 8 >= 1)
		{
			write_bits(8, bits_to_write - bits_left);
			bytes_written++;
			bits_written = 0;
			bits_left -= 8;
		}
		else
		{
			write_bits(bits_left % 8, bits_to_write - bits_left);
			bits_written += bits_left % 8;
			bits_left = 0;
		}
	}
}

void Packet::read_bits(int& data, int bits_to_read, int offset)
{
	if (bits_read >= 8)
	{
		bytes_read += bits_read / 8;
		bits_read = bits_read % 8;
	}

	//don't support cross byte writing yet
	if (bits_to_read + bits_read > 8)
	{
		throw std::exception();
	}

	for (int i = 0; i < bits_to_read; ++i)
	{
		//If this bit is 1
		int byte = static_cast<int>(bytes.data()[bytes_read - 1]);
		int shift = 1 << (bits_read + i);
		if ((byte) & (shift))
		{
			//set the other numbers bit to 1
			data |= (1 << i + offset);
		}
		else
		{
			//otherwise set it to 0
			data &= ~(1 << i + offset);
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

std::size_t Packet::get_bytes_read()
{
	return bytes_read;
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