#include "Client.hpp"

namespace enki
{
	void Client::update()
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
			on_packet_received.emit(std::move(packets.front()));
			packets.pop();
		}
	}

	void Client::pushPacket(Packet&& p)
	{
		std::lock_guard<std::mutex> lock(mutex);
		packets.push(std::move(p));
	}
}
