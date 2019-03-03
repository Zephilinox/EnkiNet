#pragma once

//LIBS
#include <enet/enet.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

//SELF
#include "Network.hpp"

class NetworkEnet : Network
{
	void start_server()
	{
		if (enet_initialized || server_initialized)
		{
			return;
		}

		auto console = spdlog::stdout_color_mt("console");

		if (int error = enet_initialize())
		{
			console->error("Enet failed to initialize");
			return;
		}
		else
		{
			enet_initialized = true;
		}

		console->info("hosting server");
		server = enet_host_create(&address, 1, 1, 0, 0);

		if (!server)
		{
			console->error("failed to start server");
		}
		else
		{
			server_initialized = true;
		}
	}

	void start_client()
	{
		if (enet_initialized || client_initialized)
		{
			return;
		}

		auto console = spdlog::stdout_color_mt("console");
		console->info("hosting client");

		enet_address_set_host(&address, "localhost");

		client = enet_host_create(nullptr, 1, 1, 0, 0);

		if (!client)
		{
			console->error("failed to start client");
		}
		else
		{
			client_initialized = true;
		}

		do
		{
			peer = enet_host_connect(client, &address, 1, 0);
			if (!peer)
			{
				console->error("client failed to connect to server");
			}
		} while (!peer);

		if (enet_host_service(client, &event, 100000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			console->info("connected to server");
			//strncpy(buffer, "client1", 1000);
			//packet = enet_packet_create(buffer, strlen(buffer) + 1, ENET_PACKET_FLAG_RELIABLE);
			//enet_peer_send(peer, 0, packet);
		}
		else
		{
			enet_peer_reset(peer);
			console->error("failed to receive connection acknoledgement from server");
		}
	}

	bool enet_initialized = false;
	bool server_initialized = false;
	bool client_initialized = false;

	ENetAddress address;
	ENetHost* server = nullptr;
	ENetHost* client = nullptr;
	ENetPeer* peer;
	ENetEvent event;
};