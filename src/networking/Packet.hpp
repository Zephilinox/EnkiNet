#pragma once

//STD
#include <vector>
#include <memory>

class Packet
{
public:
	Packet();

	Packet& operator <<(int data);
	Packet& operator >>(int& data);

private:
	template <typename T>
	void serialize(T* data);

	template <typename T>
	void deserialize(T* data);

	std::vector<std::byte> bytes;
	int bytes_written;
	int bytes_read;
};