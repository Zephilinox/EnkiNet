#include "GameData.hpp"

//STD
#include <assert.h>

GameData::GameData()
	: network_manager(this) //todo: evaluate if needed
{
}

NetworkManager* GameData::getNetworkManager() noexcept
{
	return &network_manager;
}
