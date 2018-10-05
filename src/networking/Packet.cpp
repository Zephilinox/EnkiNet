#include "Packet.hpp"

Packet::Packet()
	: bytes(1400)
{
}

Packet& Packet::operator<<(int data)
{
	memcpy(bytes.data(), &data, sizeof(data));
	return *this;
}

Packet& Packet::operator>>(int& data)
{
	memcpy(&data, bytes.data(), sizeof(data));
	return *this;
}
