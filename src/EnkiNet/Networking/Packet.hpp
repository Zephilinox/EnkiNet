#pragma once

//STD
#include <vector>
#include <memory>
#include <string>
#include <array>

namespace enki
{
	//enetpp is expecting this type. todo: change it to std::uint8_t and fix enetpp to stop warnings
	using ClientID = unsigned int;

	enum PacketType : std::uint8_t {
		NONE,

		COMMAND,

		CLIENT_INITIALIZED,
		CONNECTED,
		DISCONNECTED,

		GLOBAL_RPC,
		ENTITY_RPC,

		ENTITY_CREATION,
		ENTITY_UPDATE,
		ENTITY_DELETION
	};

	struct PacketHeader
	{
		PacketType type = PacketType::NONE;
		std::uint8_t version = 0;
		std::uint32_t timeSent = 0;
	};

	struct PacketInfo
	{
		ClientID senderID = 0;
		std::uint32_t timeReceived = 0;
	};

	class Packet
	{
	public:
		Packet(PacketHeader = {});
		Packet(const unsigned char* data, std::size_t size);

		void writeBits(int data, int bits_to_write, int offset = 0);
		[[nodiscard]] int readBits(int bits_to_read, int offset = 0);
		void writeCompressedFloat(float data, float min, float max, float resolution);
		[[nodiscard]] float readCompressedFloat(float min, float max, float resolution);

		void resetReadPosition();
		void clear();

		void setHeader(PacketHeader header);
		const PacketHeader& getHeader() const;
		const std::vector<std::byte>& getBytes() const;
		std::size_t getSize() const;
		std::size_t getBytesRead() const;

		Packet& operator <<(std::string data);
		Packet& operator >>(std::string& data);

		template <typename T>
		Packet& operator <<(T data);

		template <typename T>
		Packet& operator >>(T& data);

		template <typename T>
		Packet& operator <<(std::vector<T> data);

		template <typename T>
		Packet& operator >>(std::vector<T>& data);

		template <typename T, std::size_t size>
		Packet& operator <<(std::array<T, size> data);

		template <typename T, std::size_t size>
		Packet& operator >>(std::array<T, size>& data);

		template <typename T>
		T read();

		std::vector<std::byte> bytes;
		PacketInfo info;

	private:
		template <typename T>
		void serialize(T* data, std::size_t size);

		template <typename T>
		void deserialize(T* data, std::size_t size);

		PacketHeader header;
		std::size_t bytes_written;
		std::size_t bytes_read;
		std::size_t bits_written;
		std::size_t bits_read;
	};

	//will need to adjust this in the future for endianess concerns
	//this could potentially be dangerous depending on platform-specific sizes, not sure.
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
			//todo: do I really want to use exceptions?
			throw std::exception();
		}

		return *this;
	}

	template <typename T>
	T Packet::read()
	{
		T t;
		*this >> t;
		return t;
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
}