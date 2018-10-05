#include "Packet.hpp"

Packet::Packet()
	: bytes(1400)
	, bytes_written(0)
	, bytes_read(0)
{
}

Packet& Packet::operator<<(int data)
{
	serialize(&data);
	return *this;
}

Packet& Packet::operator>>(int& data)
{
	deserialize(&data);
	return *this;
}

template <typename T>
void Packet::serialize(T* data)
{
	memcpy(bytes.data() + bytes_written, data, sizeof(T));
	bytes_written += sizeof(T);
}

template <typename T>
void Packet::deserialize(T* data)
{
	memcpy(data, bytes.data() + bytes_read, sizeof(T));
	bytes_read += sizeof(T);
}
