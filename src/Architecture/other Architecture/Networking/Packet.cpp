#include "Packet.hpp"

Packet::Packet() noexcept
	: buffer(100) //bytes reserved, most packets will be under this amount
{
	deserializePosition = sizeof(HashedID);
	buffer.resize(sizeof(HashedID));
}

Packet::Packet(const enet_uint8* data, size_t size) noexcept
{
	deserializePosition = sizeof(HashedID);
	buffer.resize(size);
	memcpy(buffer.data(), data, size);
}

void Packet::reset()
{
	buffer.clear();

	deserializePosition = sizeof(HashedID);
	buffer.resize(sizeof(HashedID));
}

void Packet::resetSerializePosition()
{
	deserializePosition = sizeof(HashedID);
}

void Packet::setID(HashedID id) noexcept
{
	memcpy(buffer.data(), &id, sizeof(id));
}

HashedID Packet::getID() const noexcept
{
	HashedID id;
	memcpy(&id, buffer.data(), sizeof(id));
	return id;
}

//Serialization
void Packet::serialize(void* data, size_t size)
{
	//grab the old size
	auto old_size = buffer.size();
	//resize the vector so we know we have enough space to append more bits
	buffer.resize(old_size + size);
	//append the data, at the position before in the vector before it was resized
	memcpy(buffer.data() + old_size, data, size);
}

Packet& Packet::operator <<(std::string src)
{
	//correct cast?
	*this << static_cast<int32_t>(src.size());
	serialize(src.data(), src.size());
	return *this;
}

Packet& Packet::operator<<(const char* src)
{
	return *this << std::string(src);
}

Packet& Packet::operator <<(int32_t src)
{
	serialize(&src, sizeof(src));
	return *this;
}

Packet& Packet::operator <<(uint32_t src)
{
	serialize(&src, sizeof(src));
	return *this;
}

Packet& Packet::operator <<(float src)
{
	serialize(&src, sizeof(src));
	return *this;
}

Packet& Packet::operator <<(bool src)
{
	serialize(&src, sizeof(src));
	return *this;
}

void Packet::deserialize(void* destination, size_t size) noexcept
{
	memcpy(destination, buffer.data() + deserializePosition, size);
	deserializePosition += size;
}

Packet& Packet::operator >>(int32_t& destination)
{
	deserialize(&destination, sizeof(destination));
	return *this;
}

Packet& Packet::operator >>(uint32_t& destination)
{
	deserialize(&destination, sizeof(destination));
	return *this;
}

Packet& Packet::operator >>(std::string& destination)
{
	int32_t size;
	*this >> size;
	destination.resize(size);
	deserialize(destination.data(), size);
	return *this;
}

Packet& Packet::operator >>(float& destination)
{
	deserialize(&destination, sizeof(destination));
	return *this;
}

Packet& Packet::operator >>(bool& destination)
{
	deserialize(&destination, sizeof(destination));
	return *this;
}
