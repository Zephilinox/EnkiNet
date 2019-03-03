#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "../GameData.hpp"
#include "../Signals/Signal.hpp"
#include "Packet.hpp"
#include "../Timer.hpp"

class ClientInfo
{
public:
	uint32_t id;
	uint32_t get_id() const noexcept
	{
		return id;
	}
};

class Server
{
public:
	Server(GameData* game_data)
		: game_data(game_data)
	{}

	virtual ~Server() = default;

	virtual void initialize() = 0;
	virtual void deinitialize() = 0;
	virtual void processPackets() = 0;

	virtual void sendPacketToOneClient(uint32_t client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
	virtual void sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
	virtual void sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) = 0;

	virtual bool isConnected() const = 0;
	virtual bool isListening() const = 0;

	inline void update()
	{
		packetsReceived += packets.size();

		if (packetsTimer.getElapsedTime() > 10)
		{
			packetsTimer.restart();
			std::cout << "server received " << packetsReceived << " packets in the last 10 seconds\n";
			packetsReceived = 0;
		}
		
		std::lock_guard<std::mutex> guard(mutex);
		while (!packets.empty())
		{
			on_packet_received.emit(packets.front());
			packets.pop();
		}
	}

	inline void pushPacket(Packet&& p)
	{
		std::lock_guard<std::mutex> lock(mutex);
		packets.push(std::move(p));
	}

	inline bool isInitialized() const
	{
		return initialized;
	}

	Signal<Packet> on_packet_received;

protected:
	GameData* game_data;

	bool initialized;

private:
	std::mutex mutex;
	std::queue<Packet> packets;

	Timer packetsTimer;
	uint32_t packetsReceived;
};