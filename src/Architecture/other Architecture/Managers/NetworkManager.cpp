#include "NetworkManager.hpp"

//STD
#include <iostream>
#include <chrono>

//LIB
#include <jsoncons/json.hpp>

//SELF
#include "../Networking/ServerHost.hpp"
#include "../Networking/ClientHost.hpp"
#include "../Networking/ClientStandard.hpp"

using namespace std::chrono_literals;

NetworkManager::NetworkManager(GameData* game_data)
	: game_data(game_data)
{
	try
	{
		std::ifstream file("../../Resources/settings.json");
		jsoncons::json settings;
		file >> settings;
		auto netSettings = settings["Network"];

		max_clients = netSettings["Maximum Clients"].as_int();
		channel_count = netSettings["Packet Channels"].as_int();
		server_ip = netSettings["Server IP"].as_string();;
		server_port = netSettings["Server Port"].as_int();
		networkSendRate = netSettings["Packet Send Rate Per Second"].as_int();
		networkTickRate = netSettings["Packet Tick Rate Per Second"].as_int();

		std::cout << "Max Clients: " << max_clients << "\n";
		std::cout << "Max Channels: " << std::to_string(channel_count) << "\n";
		std::cout << "Server IP: " << server_ip << "\n";
		std::cout << "Server Port: " << server_port << "\n";
		std::cout << "Network Send Rate: " << networkSendRate << "\n";
		std::cout << "Network Tick Rate: " << networkTickRate << "\n";
	}
	catch (std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}

	std::cout << "Initializing global state\n";
	enetpp::global_state::get().initialize();

	network_thread = std::thread(&NetworkManager::runThreadedNetwork, this);
}

NetworkManager::~NetworkManager()
{
	std::cout << "Deinitializing global state\n";
	exit_thread = true;	
	network_thread.join();

	stopServer();
	stopClient();

	enetpp::global_state::get().deinitialize();
}

void NetworkManager::startHost()
{
	std::cout << "starting server/client\n";
	server = std::move(std::make_unique<ServerHost>(game_data));
	client = std::move(std::make_unique<ClientHost>(game_data));

	server->initialize();
	client->initialize();
}

void NetworkManager::startClient()
{
	assert(!server);

	std::cout << "starting client\n";
	client = std::move(std::make_unique<ClientStandard>(game_data));
	client->initialize();
}

void NetworkManager::stopServer()
{
	std::cout << "stopping server\n";
	if (server)
	{
		std::cout << "server exists\n";
		if (server->isInitialized())
		{
			std::cout << "server was initialized\n";
			server->deinitialize();
		}

		std::cout << "server destroyed\n";
		server.reset(nullptr);
	}
}

void NetworkManager::stopClient()
{
	std::cout << "stopping client\n";
	if (client)
	{
		std::cout << "client exists\n";
		if (client->isInitialized())
		{
			std::cout << "client was initialized\n";
			client->deinitialize();
		}

		std::cout << "client destroyed\n";
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

	if (networkSendTimer.getElapsedTime() > 1.0f / float(networkSendRate))
	{
		if ((server && client && server->isConnected()) ||
			!server && client && client->isConnected())
		{
			networkSendTimer.restart();
			on_network_tick.emit();
		}
	}
}

void NetworkManager::runThreadedNetwork()
{
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

		std::this_thread::sleep_for(1s / float(networkTickRate));
	}

	std::cout << "Network thread stopped\n";
}
