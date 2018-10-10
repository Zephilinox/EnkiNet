﻿#pragma once

//STD
#include <vector>
#include <memory>
#include <string>
#include <array>

enum PacketType : std::uint8_t {
	NONE,
	COMMAND,
	ENTITY,
	GLOBAL_RPC,
	ENTITY_RPC,
};

struct PacketHeader
{
	PacketType type = PacketType::NONE;
	std::uint8_t version = 0;
};

class Packet
{
public:

	Packet(PacketHeader = {});

	void set_header(PacketHeader header);
	const PacketHeader& get_header() const;
	const std::vector<std::byte>& get_bytes() const;

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

	template <typename T, std::size_t size>
	Packet& operator <<(std::array<T, size> data);
	template <typename T, std::size_t size>
	Packet& operator >>(std::array<T, size>& data);

private:

	template <typename T>
	void serialize(T* data, std::size_t size);

	template <typename T>
	void deserialize(T* data, std::size_t size);
	
	PacketHeader header;
	std::vector<std::byte> bytes;
	std::size_t bytes_written;
	std::size_t bytes_read;
};

//will need to adjust this in the future for endianess concerns
template <typename T>
Packet& Packet::operator <<(T data)
{
	static_assert(std::is_arithmetic_v<T>);
	serialize(&data, sizeof(T));
	return *this;
}

template <typename T>
Packet& Packet::operator >>(T& data)
{
	static_assert(std::is_arithmetic_v<T>);
	deserialize(&data, sizeof(T));
	return *this;
}

template <typename T>
Packet& Packet::operator <<(std::vector<T> data)
{
	*this << data.size();
	serialize(data.data(), sizeof(T) * data.size());
	return *this;
}

template <typename T>
Packet& Packet::operator >>(std::vector<T>& data)
{
	std::size_t size;
	*this >> size;
	data.resize(size);
	deserialize(data.data(), sizeof(T) * size);
	return *this;
}

template <typename T, std::size_t size>
Packet& Packet::operator <<(std::array<T, size> data)
{
	*this << size;
	serialize(data.data(), sizeof(T) * size);
	return *this;
}

template <typename T, std::size_t size>
Packet& Packet::operator >>(std::array<T, size>& data)
{
	std::size_t size_of_stored_array;
	*this >> size_of_stored_array;

	if (size_of_stored_array <= size)
	{
		deserialize(data.data(), size_of_stored_array);
	}
	else
	{
		//todo: how to handle errors
		throw;
	}

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
	if (bytes_read + size > bytes.size())
	{
		//todo: do I really want to use exceptions?
		throw;
	}

	memcpy(data, bytes.data() + bytes_read, size);
	bytes_read += size;
}