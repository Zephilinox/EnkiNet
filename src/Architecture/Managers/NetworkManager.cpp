#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

//SELF
#include "../Networking/Networking/ServerHost.hpp"
#include "../Networking/Networking/ClientHost.hpp"
#include "../Networking/Networking/ClientStandard.hpp"

using namespace std::chrono_literals;

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	auto console = spdlog::get("console");
	//todo
	std::cout << "Max Clients: " << max_clients << "\n";
	std::cout << "Max Channels: " << std::to_string(channel_count) << "\n";
	std::cout << "Server IP: " << server_ip << "\n";
	std::cout << "Server Port: " << server_port << "\n";
	std::cout << "Network Send Rate: " << networkTickRate << "\n";
	std::cout << "Network Tick Rate: " << networkServerTickRate << "\n";

	console->info("Initializing enet global state");
	enetpp::global_state::get().initialize();

	network_thread = std::thread(&NetworkManager::runThreadedNetwork, this);
}

NetworkManager::~NetworkManager()
{
	auto console = spdlog::get("console");
	console->info("Deinitializing global state");
	exit_thread = true;	
	network_thread.join();

	stopServer();
	stopClient();

	enetpp::global_state::get().deinitialize();
}

void NetworkManager::startHost()
{
	auto console = spdlog::get("console");
	console->info("Starting Listen Server Hosting");
	server = std::move(std::make_unique<ServerHost>(game_data));
	client = std::move(std::make_unique<ClientHost>(game_data));

	server->initialize();
	client->initialize();
}

void NetworkManager::startClient()
{
	assert(!server);

	auto console = spdlog::get("console");
	console->info("Starting Client");
	client = std::move(std::make_unique<ClientStandard>(game_data));
	client->initialize();
}

void NetworkManager::stopServer()
{
	auto console = spdlog::get("console");
	console->info("Stopping Server");
	if (server)
	{
		console->info("Server exists");
		if (server->isInitialized())
		{
			console->info("Server was initialized");
			server->deinitialize();
		}

		console->info("Server destroyed");
		server.reset(nullptr);
	}
}

void NetworkManager::stopClient()
{
	auto console = spdlog::get("console");
	console->info("Stopping Client");
	if (client)
	{
		console->info("Client exists");
		if (client->isInitialized())
		{
			console->info("Client was initialized");
			client->deinitialize();
		}

		console->info("Client destroyed");
		client.reset(nullptr);
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

	if (networkTickTimer.getElapsedTime() > 1.0f / float(networkTickRate))
	{
		if ((server && client && server->isConnected()) ||
			!server && client && client->isConnected())
		{
			networkTickTimer.restart();
			on_network_tick.emit();
		}
	}
}

void NetworkManager::runThreadedNetwork()
{
	auto console = spdlog::get("console");
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

		std::this_thread::sleep_for(1s / float(networkServerTickRate));
	}

	console->info("Network thread stopped");
}
