#pragma once

//STD
#include <vector>
#include <string>

//LIB
#include <enet\enet.h>

//SELF
//todo: take hash out of here, or replace hashedID for packets
#include "../Messages/Message.hpp"

//https://stackoverflow.com/questions/17471717/c-static-cast-from-int-to-void-to-char-can-you-help-me-to-understand-this
//http://www.cplusplus.com/forum/general/109866/
//https://stackoverflow.com/questions/2437283/c-c-packing-signed-char-into-int
//https://stackoverflow.com/questions/1874354/a-dynamic-buffer-type-in-c
//https://stackoverflow.com/questions/16543519/serialization-of-struct
//https://www.sfml-dev.org/tutorials/2.4/network-packet.php
//https://github.com/SFML/SFML/blob/master/src/SFML/Network/Packet.cpp
//https://github.com/godotengine/godot/blob/master/modules/enet/networked_multiplayer_enet.cpp

struct Packet
{
	Packet() noexcept;
	Packet(const enet_uint8* data, size_t size) noexcept;
	
	void reset();
	void resetSerializePosition();

	void setID(HashedID id) noexcept;
	HashedID getID() const noexcept;

	void serialize(void* data, size_t size);
	Packet& operator <<(std::string src);
	Packet& operator <<(const char* src);
	Packet& operator <<(int32_t src);
	Packet& operator <<(uint32_t src);
	Packet& operator <<(float src);
	Packet& operator <<(bool src);

	void deserialize(void* destination, size_t size) noexcept;
	Packet& operator >>(int32_t& destination);
	Packet& operator >>(uint32_t& destination);
	Packet& operator >>(std::string& destination);
	Packet& operator >>(float& destination);
	Packet& operator >>(bool& destination);

	std::vector<enet_uint8> buffer;
	size_t deserializePosition = 0;

	uint32_t senderID;
};