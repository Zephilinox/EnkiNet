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

	std::unique_ptr<Server> server = nullptr;
	std::unique_ptr<Client> client = nullptr;

	inline uint32_t getMaxClients()
	{
		return max_clients;
	}

	inline uint8_t getChannelCount()
	{
		return channel_count;
	}

	inline const char* getServerIP()
	{
		return server_ip.c_str();
	}

	inline uint32_t getServerPort()
	{
		return server_port;
	}

	Signal<> on_network_tick;
	
private:
	void runThreadedNetwork();

	GameData* game_data;
	std::thread network_thread;
	bool exit_thread = false;

	uint32_t max_clients = 2;
	uint8_t channel_count = 1;
	std::string server_ip = "localhost";
	uint32_t server_port = 22222;

	Timer networkSendTimer;
	int networkSendRate = 10;

	int networkTickRate = 60;
};