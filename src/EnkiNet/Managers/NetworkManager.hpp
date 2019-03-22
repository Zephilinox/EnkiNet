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

namespace enki
{
	class Server;
	class Client;

	class NetworkManager
	{
	public:
		NetworkManager();
		~NetworkManager();

		void startHost();
		void startClient();

		void stopServer();
		void stopClient();

		void update();

		std::unique_ptr<Server> server;
		std::unique_ptr<Client> client;
		Signal<> on_network_tick;
		int network_send_rate = 60;
		int network_process_rate = 120;

		//note: doesn't include the server even if its a player too
		uint32_t max_clients = 7;
		uint8_t channel_count = 1;
		std::string server_ip = "localhost";
		uint16_t server_port = 22222;

	private:
		void runThreadedNetwork();

		std::thread network_thread;
		bool exit_thread = false;
		Timer network_process_timer;
	};
}