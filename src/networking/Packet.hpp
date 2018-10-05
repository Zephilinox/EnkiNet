#pragma once

//STD
#include <vector>
#include <memory>
#include <string>

class Packet
{
public:
	Packet();

	template <typename T>
	Packet& operator <<(T data);

	template <typename T>
	Packet& operator >>(T& data);

	Packet& operator <<(std::string data);
	Packet& operator >>(std::string& data);

	template <typename T>
	Packet& operator <<(std::vector<T> data);

	template <typename T>
	Packet& operator >>(std::vector<T>& data);

private:
	template <typename T>
	void serialize(T* data, std::size_t size);

	template <typename T>
	void deserialize(T* data, std::size_t size);
	
	std::vector<std::byte> bytes;
	int bytes_written;
	int bytes_read;
};

//will need to adjust this in the future for endianess concerns
template <typename T>
Packet& Packet::operator<<(T data)
{
	static_assert(std::is_arithmetic_v<T>);
	serialize(&data, sizeof(T));
	return *this;
}

template <typename T>
Packet& Packet::operator>>(T& data)
{
	static_assert(std::is_arithmetic_v<T>);
	deserialize(&data, sizeof(T));
	return *this;
}

template <typename T>
Packet& Packet::operator<<(std::vector<T> data)
{
	*this << data.size();
	serialize(data.data(), sizeof(T) * data.size());
	return *this;
}

template <typename T>
Packet& Packet::operator>>(std::vector<T>& data)
{
	std::size_t size;
	*this >> size;
	data.resize(size);
	deserialize(data.data(), sizeof(T) * size);
	return *this;
}

template <typename T>
void Packet::serialize(T* data, std::size_t size)
{
	if (bytes_written + size > bytes.size())
	{
		bytes.resize((bytes_written + size) * 2);
	}

	memcpy(bytes.data() + bytes_written, data, size);
	bytes_written += size;
}

template <typename T>
void Packet::deserialize(T* data, std::size_t size)
{
	memcpy(data, bytes.data() + bytes_read, size);
	bytes_read += size;
}