#include "InputManager.hpp"

//STD
#include <cassert>

//LIB
#include <jsoncons/json.hpp>

/**
*   @brief   Constructor.
*   @details Initializes the recorded state of all keys
*/
InputManager::InputManager(ASGE::Input* input) noexcept
	: input(input)
{
	toggle_keys.fill(ASGE::KEYS::KEY_RELEASED);
	keys.fill(ASGE::KEYS::KEY_RELEASED);

	gamepad_buttons_last_frame.fill(ASGE::KEYS::KEY_RELEASED);
	gamepad_buttons.fill(ASGE::KEYS::KEY_RELEASED);

	mouse_buttons_last_frame.fill(ASGE::KEYS::KEY_RELEASED);
	mouse_buttons.fill(ASGE::KEYS::KEY_RELEASED);
	mouse_buttons_this_frame.fill(ASGE::KEYS::KEY_RELEASED);

	callback_id = input->addCallbackFnc(ASGE::EventType::E_GAMEPAD_STATUS, &InputManager::gamepadHandler, this);
	callback_id2 = input->addCallbackFnc(ASGE::EventType::E_MOUSE_CLICK, &InputManager::mouseHandler, this);
	callback_id3 = input->addCallbackFnc(ASGE::EventType::E_KEY, &InputManager::keyHandler, this);

	try
	{
		std::ifstream file("../../Resources/settings.json");

		jsoncons::json settings;
		file >> settings;

		gamepad_button_up = settings["GamePad"]["Up"].as_int() + ASGE::KEYS::KEY_LAST;
		gamepad_button_down = settings["GamePad"]["Down"].as_int() + ASGE::KEYS::KEY_LAST;
		gamepad_button_enter = settings["GamePad"]["Enter"].as_int() + ASGE::KEYS::KEY_LAST;
		gamepad_button_escape = settings["GamePad"]["Escape"].as_int() + ASGE::KEYS::KEY_LAST;
		std::cout << "GamePadUp: " << settings["GamePad"]["Up"].as_int() << "\n";
		std::cout << "GamePadDown: " << settings["GamePad"]["Down"].as_int() << "\n";
		std::cout << "GamePadEnter: " << settings["GamePad"]["Enter"].as_int() << "\n";
		std::cout << "GamePadEscape: " << settings["GamePad"]["Escape"].as_int() << "\n";
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR INFO: " << e.what() << "\n";
	}
}

InputManager::~InputManager()
{
	input->unregisterCallback(callback_id);
	input->unregisterCallback(callback_id2);
}


void InputManager::update()
{
	{
		std::lock_guard<std::mutex> guard(keys_mutex);
		for (auto& key : keys)
		{
			//If the key hasn't been released since the last update
			//Then set it to repeated so that
			//KeyDown returns true but 
			//KeyPressed returns false
			if (key == ASGE::KEYS::KEY_PRESSED)
			{
				key = ASGE::KEYS::KEY_REPEATED;
			}
		}
	}

	auto game_pad = getGamePad();

	//http://www.glfw.org/docs/3.3/group__gamepad__buttons.html ?
	if (game_pad.is_connected)
	{
		assert(game_pad.no_of_buttons < ASGE::KEYS::KEY_LAST);

		for (int i = ASGE::KEYS::KEY_LAST; i < game_pad.no_of_buttons + ASGE::KEYS::KEY_LAST; ++i)
		{
			gamepad_buttons_last_frame[i] = gamepad_buttons[i];
			gamepad_buttons[i] = game_pad.buttons[i - ASGE::KEYS::KEY_LAST];
		}
	}

	{
		std::lock_guard<std::mutex> lock(mouse_mutex);
		for (size_t i = 0; i < mouse_buttons_last_frame.size(); ++i)
		{
			mouse_buttons_last_frame[i] = mouse_buttons_this_frame[i];
			mouse_buttons_this_frame[i] = mouse_buttons[i];
		}
	}
}

/**
*   @brief   Updates the recorded state of the key.
*   @details Should be called by a function registered as a callback via ASGE::Input.
Or as a direct callback itself.
*   @param   key is the key that will be changed
*   @param   state is the new state of the key, such as ASGE::KEYS::KEY_PRESSED
*   @return void*/
void InputManager::handleInput(int key, int state)
{
	//Fixes Alt+PrintScreen
	if (key >= 0)
	{
		std::lock_guard<std::mutex> guard(keys_mutex);
		toggle_keys[key] = state;
		keys[key] = state;
	}
}

void InputManager::addAction(HashedID action, unsigned id)
{
	actions.insert({ action, id });
}

bool InputManager::removeAction(HashedID action, unsigned id)
{
	bool removed = false;

	std::experimental::erase_if(actions, [&](const auto& key_val)
	{
		if (key_val.first == action && key_val.second == id)
		{
			removed = true;
			return true;
		}

		return false;
	});

	return removed;
}

bool InputManager::isActionPressed(HashedID action)
{
	auto range = actions.equal_range(action);

	bool pressed = false;

	for (auto it = range.first; it != range.second; ++it)
	{
		if (it->second > 0 && it->second < ASGE::KEYS::KEY_LAST)
		{
			pressed = pressed || isKeyPressed(it->second);
		}
		else
		{
			pressed = pressed || isGamePadButtonPressed(it->second);
		}
	}

	return pressed;
}

bool InputManager::isActionDown(HashedID action)
{
	auto range = actions.equal_range(action);

	bool down = false;

	for (auto it = range.first; it != range.second; ++it)
	{
		if (it->second > 0 && it->second < ASGE::KEYS::KEY_LAST)
		{
			down = down || isKeyDown(it->second);
		}
		else
		{
			down = down || isGamePadButtonDown(it->second);
		}
	}

	return down;
}


/**
*   @brief   Determines if the key was pressed this frame
*   @param   key is the key whose state is being checked
*   @return  true if the key was pressed since the last call to update. */
bool InputManager::isKeyPressed(int key)
{
	if (toggle_keys[key] == ASGE::KEYS::KEY_PRESSED)
	{
		std::lock_guard<std::mutex> guard(keys_mutex);
		toggle_keys[key] = ASGE::KEYS::KEY_RELEASED;
		keys[key] = ASGE::KEYS::KEY_PRESSED;
		return true;
	}

	return (keys[key] == ASGE::KEYS::KEY_PRESSED);
}


/**
*   @brief   Determines if the key is being held down
*   @param   key is the key whose state is being checked
*   @return  true if the key is being held down. */
bool InputManager::isKeyDown(int key) noexcept
{
	//Returns true until the key is released
	[[gsl::suppress(bounds.2)]]
	return (toggle_keys[key] != ASGE::KEYS::KEY_RELEASED ||
		keys[key] != ASGE::KEYS::KEY_RELEASED);
}

bool InputManager::isGamePadButtonPressed(int button)
{
	return gamepad_buttons_last_frame[button] == ASGE::KEYS::KEY_RELEASED &&
		gamepad_buttons_last_frame[button] != gamepad_buttons[button];
}

bool InputManager::isGamePadButtonDown(int button)
{
	return gamepad_buttons[button] == ASGE::KEYS::KEY_PRESSED;
}

bool InputManager::isMouseButtonPressed(int button)
{
	return mouse_buttons_this_frame[button] == ASGE::KEYS::KEY_PRESSED &&
		mouse_buttons_last_frame[button] != mouse_buttons_this_frame[button];
}

bool InputManager::isMouseButtonReleased(int button)
{
	return mouse_buttons_this_frame[button] == ASGE::KEYS::KEY_RELEASED &&
		mouse_buttons_last_frame[button] != mouse_buttons_this_frame[button];
}

bool InputManager::isMouseButtonDown(int button)
{
	return mouse_buttons[button] == ASGE::KEYS::KEY_PRESSED;
}

void InputManager::getMouseScreenPosition(double& xpos, double& ypos)
{
	input->getCursorPos(xpos, ypos);
}

void InputManager::getMouseWorldPosition(double & xpos, double & ypos)
{
	input->getCursorPos(xpos, ypos);
	xpos += offset_x;
	ypos += offset_y;
}

void InputManager::applyOffset(float x, float y)
{
	offset_x = x;
	offset_y = y;
}

GamePadData InputManager::getGamePad()
{
	return input->getGamePad(0);
}

void InputManager::gamepadHandler(const ASGE::SharedEventData data)
{
	const auto gamepad_event = static_cast<const ASGE::GamePadEvent*>(data.get());

	//todo: what's the point of this?
	std::cout << "Gamepad Event Received (is connected) " << gamepad_event->connected;
}

void InputManager::mouseHandler(const ASGE::SharedEventData data)
{
	const auto mouse_event = static_cast<const ASGE::ClickEvent*>(data.get());
	std::lock_guard<std::mutex> guard(mouse_mutex);
	mouse_buttons[mouse_event->button] = mouse_event->action;
}

void InputManager::keyHandler(const ASGE::SharedEventData data)
{
	const ASGE::KeyEvent* key_event = static_cast<const ASGE::KeyEvent*>(data.get());
	handleInput(key_event->key, key_event->action);
}
