#pragma once

//LIB
#include <Engine/Renderer.h>

//SELF
#include "Managers/InputManager.hpp"
#include "Managers/StateManager.hpp"
#include "Managers/FontManager.hpp"
#include "Messages/MessageQueue.hpp"
#include "Managers/NetworkManager.hpp"
#include "Audio/MusicPlayer.hpp"
#include "Rng.hpp"

class GameData
{
public:
	GameData(ASGE::Renderer* renderer, ASGE::Input* input, int width, int height);

	ASGE::Renderer* getRenderer() const noexcept;
	ASGE::Input* getInput() noexcept;
	StateManager* getStateManager() noexcept;
	InputManager* getInputManager() noexcept;
	FontManager* getFontManager() noexcept;
	MessageQueue* getMessageQueue() noexcept;
	NetworkManager* getNetworkManager() noexcept;
	Rng* getRandomNumberGenerator() noexcept;
	MusicPlayer* getMusicPlayer() noexcept;

	int getWindowWidth() noexcept;
	int getWindowHeight() noexcept;

	void toggleFullscreen() noexcept;

	bool exit = false;

private:
	ASGE::Renderer* renderer = nullptr;
	ASGE::Input* input = nullptr;

	/** Input Manager. A wrapper around key states from callbacks by ASGE::Input. */
	//This can be removed, just make sure that input is not multithreaded and add new functions to states to handle events.
	//Alternatively convert ASGE events and push them to our queue by using a wrapper, but that shouldn't be necessary.
	InputManager input_manager;

	/** State Manager. A wrapper around storing states in a stack. */
	StateManager state_manager;

	/* Works a little differently to how ASGE works
	Make sure you add a font and that you load any font sizes you need before you set the font as active*/
	FontManager font_manager;

	/*This is pretty neat but we likely won't need to use it.
	Essentially this would let us heavily decouple on class from another by sending events to the queue
	The other class (or classes) can grab these events and handle them via a function.
	For instance, an achievement system could read events pushed to the message queue by the dialogue system*/
	MessageQueue message_queue;

	Rng random_number_gen;

	NetworkManager network_manager;

	MusicPlayer music_player;

	int window_width;
	int window_height;
};
