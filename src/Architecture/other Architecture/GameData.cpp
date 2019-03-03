#include "GameData.hpp"

//STD
#include <assert.h>

//LIB
#include <jsoncons/json.hpp>

//SELF
#include "Audio/AudioLocator.hpp"

GameData::GameData(ASGE::Renderer* renderer, ASGE::Input* input, int width, int height)
	: renderer(renderer)
	, input(input)
	, state_manager(this)
	, input_manager(input)
	, font_manager(renderer)
	, network_manager(this) //todo: evaluate if needed
	, window_width(width)
	, window_height(height)
{
	assert(renderer);

	try
	{
		std::ifstream file("../../Resources/settings.json");
		jsoncons::json settings;
		file >> settings;
		AudioLocator::set(settings["Audio"]["Engine"].as_int());
		std::cout << "Audio Engine: " << settings["Audio"]["Engine"].as_int() << "\n";
	}
	catch (std::runtime_error& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

ASGE::Renderer* GameData::getRenderer() const noexcept
{
	return renderer;
}

ASGE::Input* GameData::getInput() noexcept
{
	return input;
}

InputManager* GameData::getInputManager() noexcept
{
	return &input_manager;
}

FontManager* GameData::getFontManager() noexcept
{
	return &font_manager;
}

MessageQueue* GameData::getMessageQueue() noexcept
{
	return &message_queue;
}

StateManager* GameData::getStateManager() noexcept
{
	return &state_manager;
}

Rng* GameData::getRandomNumberGenerator() noexcept
{
	return &random_number_gen;
}

MusicPlayer* GameData::getMusicPlayer() noexcept
{
	return &music_player;
}

NetworkManager* GameData::getNetworkManager() noexcept
{
	return &network_manager;
}

int GameData::getWindowWidth() noexcept
{
	return window_width;
}

int GameData::getWindowHeight() noexcept
{
	return window_height;
}

void GameData::toggleFullscreen() noexcept
{
	if (renderer->getWindowMode() == ASGE::Renderer::WindowMode::WINDOWED)
	{
		renderer->setWindowedMode(ASGE::Renderer::WindowMode::FULLSCREEN);
	}
	else
	{
		renderer->setWindowedMode(ASGE::Renderer::WindowMode::WINDOWED);
	}
}
