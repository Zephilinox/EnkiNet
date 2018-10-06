#pragma once

//STD
#include <vector>
#include <memory>
#include <string>
#include <array>

struct PacketHeaderEnki;

template <class PacketHeader = PacketHeaderEnki>
class Packet
{
public:
	template <typename... Args>
	Packet(Args... args);

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

	void set_header(PacketHeader p_header)
	{
		header = p_header;
		memcpy(bytes.data(), &header, sizeof(PacketHeader));
	}

	const PacketHeader& get_header() const
	{
		return header;
	}

	const std::vector<std::byte>& get_bytes() const
	{
		return bytes;
	}

private:
	template <typename T>
	void serialize(T* data, std::size_t size);

	template <typename T>
	void deserialize(T* data, std::size_t size);

	PacketHeader header;
	std::vector<std::byte> bytes;
	int bytes_written;
	int bytes_read;
};

template <class PacketHeader>
template <typename... Args>
Packet<PacketHeader>::Packet(Args... args)
	: bytes(sizeof(PacketHeader))
	, bytes_written(sizeof(PacketHeader))
	, bytes_read(sizeof(PacketHeader))
	, header{args...}
{
	bytes.reserve(1400);
	memcpy(bytes.data(), &header, sizeof(PacketHeader));
}

//will need to adjust this in the future for endianess concerns
template <class PacketHeader>
template <typename T>
Packet<PacketHeader>& Packet<PacketHeader>::operator <<(T data)
{
	static_assert(std::is_arithmetic_v<T>);
	serialize(&data, sizeof(T));
	return *this;
}

template <class PacketHeader>
template <typename T>
Packet<PacketHeader>& Packet<PacketHeader>::operator >>(T& data)
{
	static_assert(std::is_arithmetic_v<T>);
	deserialize(&data, sizeof(T));
	return *this;
}

template <class PacketHeader>
Packet<PacketHeader>& Packet<PacketHeader>::operator <<(std::string data)
{
	*this << data.length();
	serialize(data.data(), data.length());
	return *this;
}

template <class PacketHeader>
Packet<PacketHeader>& Packet<PacketHeader>::operator >>(std::string& data)
{
	std::size_t length;
	*this >> length;
	data.resize(length);
	deserialize(data.data(), length);
	return *this;
}

template <class PacketHeader>
template <typename T>
Packet<PacketHeader>& Packet<PacketHeader>::operator <<(std::vector<T> data)
{
	*this << data.size();
	serialize(data.data(), sizeof(T) * data.size());
	return *this;
}

template <class PacketHeader>
template <typename T>
Packet<PacketHeader>& Packet<PacketHeader>::operator >>(std::vector<T>& data)
{
	std::size_t size;
	*this >> size;
	data.resize(size);
	deserialize(data.data(), sizeof(T) * size);
	return *this;
}

template <class PacketHeader>
template <typename T, std::size_t size>
Packet<PacketHeader>& Packet<PacketHeader>::operator <<(std::array<T, size> data)
{
	*this << size;
	serialize(data.data(), sizeof(T) * size);
	return *this;
}

template <class PacketHeader>
template <typename T, std::size_t size>
Packet<PacketHeader>& Packet<PacketHeader>::operator >>(std::array<T, size>& data)
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

template <class PacketHeader>
template <typename T>
void Packet<PacketHeader>::serialize(T* data, std::size_t size)
{
	if (bytes_written + size > bytes.size())
	{
		bytes.resize((bytes_written + size) * 2);
	}

	memcpy(bytes.data() + bytes_written, data, size);
	bytes_written += size;
}

template <class PacketHeader>
template <typename T>
void Packet<PacketHeader>::deserialize(T* data, std::size_t size)
{
	if (bytes_read + size > bytes.size())
	{
		//todo: do I really want to use exceptions?
		throw;
	}

	memcpy(data, bytes.data() + bytes_read, size);
	bytes_read += size;
}

struct PacketHeaderEnki
{
	enum Type : std::uint8_t
	{
		NONE,
		ENTITY,
		RPC,
	};

	Type type = Type::NONE;
};