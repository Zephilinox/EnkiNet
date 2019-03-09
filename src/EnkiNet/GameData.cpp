#include "GameData.hpp"

GameData::GameData()
	: network_manager(this) //todo: evaluate if needed
{
}

NetworkManager* GameData::getNetworkManager() noexcept
{
	return &network_manager;
}
