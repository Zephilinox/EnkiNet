#pragma once

namespace enki
{
	class Scenegraph;
	class NetworkManager;

	struct GameData
	{
	public:
		NetworkManager* network_manager = nullptr;
		Scenegraph* scenegraph = nullptr;
		void* custom = nullptr;
	};
}