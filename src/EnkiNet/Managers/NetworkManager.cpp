#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

//LIBS
#include <spdlog/spdlog.h>

//SELF
#include "../Networking/Networking/ServerHost.hpp"
#include "../Networking/Networking/ClientHost.hpp"
#include "../Networking/Networking/ClientStandard.hpp"

namespace enki
{
	using namespace std::chrono_literals;

	NetworkManager::NetworkManager(GameData* game_data)
		: game_data(game_data)
	{
		spdlog::stdout_color_mt("EnkiNet");
		auto console = spdlog::get("EnkiNet");
		//todo
		std::cout << "Max Clients: " << max_clients << "\n";
		std::cout << "Max Channels: " << std::to_string(channel_count) << "\n";
		std::cout << "Server IP: " << server_ip << "\n";
		std::cout << "Server Port: " << server_port << "\n";
		std::cout << "Network Send Rate: " << network_send_rate << "\n";
		std::cout << "Network Tick Rate: " << network_process_rate << "\n";

		console->info("Initializing enet global state");
		enetpp::global_state::get().initialize();

		network_thread = std::thread(&NetworkManager::runThreadedNetwork, this);
	}

	NetworkManager::~NetworkManager()
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Deinitializing global state");
		exit_thread = true;
		network_thread.join();

		stopServer();
		stopClient();

		enetpp::global_state::get().deinitialize();
	}

	void NetworkManager::startHost()
	{
		assert(!server);
		assert(!client);

		auto console = spdlog::get("EnkiNet");
		console->info("Starting Listen Server Hosting");
		server = std::make_unique<ServerHost>(game_data);
		client = std::make_unique<ClientHost>(game_data);
	}

	void NetworkManager::startClient()
	{
		assert(!server);
		assert(!client);

		auto console = spdlog::get("EnkiNet");
		console->info("Starting Client");
		client = std::move(std::make_unique<ClientStandard>(game_data));
	}

	void NetworkManager::stopServer()
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Stopping Server");
		if (server)
		{
			console->info("Server exists");
			server.reset(nullptr);
			console->info("Server destroyed");
		}
	}

	void NetworkManager::stopClient()
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Stopping Client");
		if (client)
		{
			console->info("Client exists");
			client.reset(nullptr);
			console->info("Client destroyed");
		}
	}

	void NetworkManager::update()
	{
		if (server)
		{
			server->update();
		}

		if (client)
		{
			client->update();
		}

		if (network_process_timer.getElapsedTime() > 1.0f / float(network_send_rate))
		{
			if ((server && (!server->getConnectedClients().empty() || client)) ||
				!server && client && client->isConnected())
			{
				network_process_timer.restart();
				on_network_tick.emit();
			}
		}
	}

	void NetworkManager::runThreadedNetwork()
	{
		auto console = spdlog::get("EnkiNet");
		console->info("Network thread started");
		while (!exit_thread)
		{
			if (server)
			{
				server->processPackets();
			}

			if (client)
			{
				client->processPackets();
			}

			std::this_thread::sleep_for(1s / float(network_process_rate));
		}

		console->info("Network thread stopped");
	}
}