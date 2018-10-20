#pragma once

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

	void write_bits(int& data, int bits_to_write, int offset = 0);
	void read_bits(int& data, int bits_to_read, int offset = 0);
	void write_compressed_float(float& data, float min, float max, float resolution)
	{
		//min = -10, max = 10, resolution = 0.01
		//20
		float delta = max - min;
		//2000
		float total_possible_values = delta / resolution;
		//2000
		int max_possible_values = std::ceil(total_possible_values);
		//11
		int bits_required = static_cast<int>(std::ceil(std::log(max_possible_values) / std::log(2)));
		//data = 5, normalized = 0.75
		float normalized_data = std::clamp((data - min) / delta, 0.0f, 1.0f);
		//1500
		int final_value = static_cast<int>(std::round(normalized_data * max_possible_values));
		//11 bits written vs standard 32 for a float
		write_bits(final_value, bits_required);
	}

	void read_compressed_float(float& data, float min, float max, float resolution)
	{
		//min = -10, max = 10, resolution = 0.01
		//20
		float delta = max - min;
		//2000
		float total_possible_values = delta / resolution;
		//2000
		int max_possible_values = std::ceil(total_possible_values);
		//11
		int bits_required = static_cast<int>(std::ceil(std::log(max_possible_values) / std::log(2)));

		//5
		int final_value = 0;
		read_bits(final_value, bits_required);

		//final_value = 5, normalized = 0.75
		float normalized_data = static_cast<float>(final_value) / static_cast<float>(max_possible_values);
		//0.75 * 20 - 10 = 15 - 10 = 5
		data = normalized_data * delta + min;
	}

	void set_header(PacketHeader header);
	const PacketHeader& get_header() const;
	const std::vector<std::byte>& get_bytes() const;
	std::size_t get_bytes_read();

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
	std::size_t bits_written;
	std::size_t bits_read;
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
	bits_written = 8;
}

template <typename T>
void Packet::deserialize(T* data, std::size_t size)
{
	if (bytes_read + size > bytes.size())
	{
		//todo: do I really want to use exceptions?
		throw std::exception();
	}

	memcpy(data, bytes.data() + bytes_read, size);
	bytes_read += size;
	bits_read = 8;
}