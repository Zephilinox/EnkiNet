#pragma once

//STD
#include <iostream>

//LIB
#include <enetpp/client.h>

//SELF
#include "../../Signals/Signal.hpp"
#include "../Packet.hpp"
#include "../../Timer.hpp"
#include "../../GameData.hpp"

class Client
{
public:
	Client(GameData* game_data)
		: game_data(game_data)
	{}

	virtual ~Client() = default;

	virtual void processPackets() = 0;

	virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;

	virtual bool isConnected() const = 0;
	virtual bool isConnecting() const = 0;
	
	inline void update()
	{
		std::lock_guard<std::mutex> guard(mutex);

		packetsReceived += packets.size();

		if (packetsTimer.getElapsedTime() > 10)
		{
			auto console = spdlog::get("EnkiNet");
			packetsTimer.restart();
			console->info("client received {} packets in the last 10 seconds", packetsReceived);
			packetsReceived = 0;
		}

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
	
	inline ClientID getID() const
	{
		return id;
	}

	Signal<Packet> on_packet_received;

protected:
	GameData* game_data;
	ClientID id = 0;

private:
	std::mutex mutex;
	std::queue<Packet> packets;

	Timer packetsTimer;
	uint32_t packetsReceived = 0;
};