#pragma once

//STD
#include <iostream>

//LIB
#include <enetpp/client.h>
#include <spdlog/spdlog.h>

//SELF
#include "EnkiNet/Signals/Signal.hpp"
#include "EnkiNet/Networking/Packet.hpp"
#include "EnkiNet/Timer.hpp"

namespace enki
{
	class Client
	{
	public:
		virtual ~Client() = default;

		virtual void processPackets() = 0;

		virtual void sendPacket(enet_uint8 channel_id, Packet* p, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) = 0;

		virtual bool isConnected() const = 0;
		virtual bool isConnecting() const = 0;

		void update();
		void pushPacket(Packet&& p);

		inline ClientID getID() const
		{
			return id;
		}

		Signal<Packet> on_packet_received;

	protected:
		Client() = default;
		ClientID id = 0;

	private:
		std::mutex mutex;
		std::queue<Packet> packets;

		Timer packetsTimer;
		uint32_t packetsReceived = 0;
	};
}