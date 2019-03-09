#pragma once

//STD
#include <functional>
#include <thread>

//LIB
#include <enetpp/client.h>
#include <enetpp/server.h>

//SELF
#include "../Signals/Signal.hpp"
#include "../Timer.hpp"

class GameData;
class Server;
class Client;

class NetworkManager
{
public:
	NetworkManager(GameData* game_data);
	~NetworkManager();

	void startHost();
	void startClient();
	
	void stopServer();
	void stopClient();

	void update();

	inline uint32_t getMaxClients() const
	{
		return max_clients;
	}

	inline uint8_t getChannelCount() const
	{
		return channel_count;
	}

	inline const char* getServerIP() const
	{
		return server_ip.c_str();
	}

	inline uint16_t getServerPort() const
	{
		return server_port;
	}


	std::unique_ptr<Server> server;
	std::unique_ptr<Client> client;
	Signal<> on_network_tick;
	int networkTickRate = 60;

private:
	void runThreadedNetwork();

	GameData* game_data;
	std::thread network_thread;
	bool exit_thread = false;

	uint32_t max_clients = 2;
	uint8_t channel_count = 1;
	std::string server_ip = "localhost";
	uint16_t server_port = 22222;

	Timer networkTickTimer;

	int networkServerTickRate = 120;
};
