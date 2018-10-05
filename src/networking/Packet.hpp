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
	std::vector<std::byte> bytes;
};