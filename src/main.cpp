//STD
#include <string>
#include <cassert>
#include <iostream>
#include <map>
#include <functional>
#include <any>
#include <type_traits>

//LIBS
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <enet/enet.h>

//SELF
#include "networking/Packet.hpp"
#include "networking/RPC.hpp"

//TESTS
#include "networking/PacketTest.hpp"
#include "networking/RPCTest.hpp"

int main(int argc, char** argv)
{
	doctest::Context context;
	context.setOption("abort-after", 5);
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);
	int result = context.run();
	if (context.shouldExit())
	{
		return result;
	}

	auto console = spdlog::stdout_color_mt("console");
	console->info("Hi :)");

	char n;
	do
	{
		std::cout << "'s' for server, 'c' for client: ";
		std::cin >> n;
	}
	while (n != 's' && n != 'c');

	bool is_server;
	if (n == 's')
	{
		is_server = true;
	}
	else
	{
		is_server = false;
	}

	if (int error = enet_initialize())
	{
		console->error("Enet failed to initialize");
	}

	ENetAddress address{ ENET_HOST_ANY, 1234 };
	ENetHost* server = nullptr;
	ENetHost* client = nullptr;
	ENetPeer* peer;
	ENetEvent event;
	ENetPacket* packet;
	char buffer[1000];

	bool success = false;

	if (is_server)
	{
		console->info("hosting server");
		server = enet_host_create(&address, 1, 1, 0, 0);

		if (!server)
		{
			console->error("failed to start server");
		}
		else
		{
			success = true;
		}
	}
	else
	{
		console->info("hosting client");

		enet_address_set_host(&address, "localhost");

		client = enet_host_create(nullptr, 1, 1, 0, 0);

		if (!client)
		{
			console->error("failed to start client");
		}
		else
		{
			success = true;
		}

		do
		{
			peer = enet_host_connect(client, &address, 1, 0);
			if (!peer)
			{
				console->error("client failed to connect to server");
			}
		} while (!peer);

		if (enet_host_service(client, &event, 1000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			console->info("connected to server");
			strncpy(buffer, "client1", 1000);
			packet = enet_packet_create(buffer, strlen(buffer) + 1, ENET_PACKET_FLAG_RELIABLE);
			enet_peer_send(peer, 0, packet);
			success = true;
		}
		else
		{
			enet_peer_reset(peer);
			console->error("failed to receive connection ackoledgement from server");
			success = false;
		}
	}

	bool hosting = success;
	while (hosting)
	{
		if (server)
		{
			while (enet_host_service(server, &event, 0) > 0)
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_CONNECT:
					{
						std::cout << "server connect\n";
						char host_name[1000];
						enet_address_get_host(&event.peer->address, host_name, 1000);

						console->info("Client connected from {}:{}", host_name, event.peer->address.port);
						break;
					}
					case ENET_EVENT_TYPE_RECEIVE:
					{
						std::cout << "server received\n";

						//use fmt instead of the console since for some reason it doesn't like being passed a null pointer
						fmt::print("A packet of length {} containing {} was received from {} on channel {}.\n",
							event.packet->dataLength,
							event.packet->data,
							event.peer->data,
							event.channelID);

						enet_packet_destroy(event.packet);
						break;
					}
					case ENET_EVENT_TYPE_DISCONNECT:
					{
						std::cout << "server disconnect\n";
						console->info("{} disconnected", event.peer->data);
						event.peer->data = 0;
						break;
					}
				}
			}
		}
		else if (client)
		{
			while (enet_host_service(client, &event, 0) > 0)
			{
				switch (event.type)
				{
					case ENET_EVENT_TYPE_RECEIVE:
					{
						std::cout << "client receive\n";
						console->info("A packet of length {} containing {} was received from {} on channel {}.\n",
							event.packet->dataLength,
							event.packet->data,
							event.peer->data,
							event.channelID);

						enet_packet_destroy(event.packet);

						break;
					}

					case ENET_EVENT_TYPE_DISCONNECT:
					{
						std::cout << "client disconnect\n";
						console->info("{} disconnected.\n", event.peer->data);
						event.peer->data = 0;
						return 0;
					}
				}
			}
		}
	}

	enet_host_destroy(server);
	enet_host_destroy(client);
	enet_deinitialize();

	while (true);

	return 0;
}