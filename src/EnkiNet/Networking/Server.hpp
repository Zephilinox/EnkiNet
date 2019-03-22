#pragma once

//LIB
#include <enetpp/client.h>

//SELF
#include "Signals/Signal.hpp"
#include "Packet.hpp"
#include "Timer.hpp"

namespace enki
{
	class ClientInfo
	{
	public:
		ClientID id;
		ClientID get_id() const noexcept
		{
			return id;
		}
	};

	class Server
	{
	public:
		Server() = default;
		virtual ~Server() = default;

		virtual void processPackets() = 0;

		virtual void sendPacketToOneClient(ClientID client_id, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
		virtual void sendPacketToAllClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;
		virtual void sendPacketToSomeClients(enet_uint8 channel_id, Packet* p, enet_uint32 flags, std::function<bool(const ClientInfo& client)> predicate) = 0;
		virtual void sendPacketToAllExceptOneClient(ClientID client_id_excluded, enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;

		virtual bool isListening() const = 0;
		virtual const std::vector<ClientInfo*>& getConnectedClients() const = 0;

		inline void update()
		{
			packetsReceived += packets.size();

			if (packetsTimer.getElapsedTime() > 10)
			{
				auto console = spdlog::get("EnkiNet");
				packetsTimer.restart();
				console->info("server received {} packets in the last 10 seconds", packetsReceived);
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

		Signal<Packet> on_packet_received;
	
	private:
		std::mutex mutex;
		std::queue<Packet> packets;

		Timer packetsTimer;
		uint32_t packetsReceived = 0;
	};
}