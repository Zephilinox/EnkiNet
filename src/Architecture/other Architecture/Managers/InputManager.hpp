#pragma once

//STD
#include <array>
#include <mutex>
#include <string>
#include <map>
#include <experimental/map>

//LIB
#include <Engine/Keys.h>
#include <Engine/Input.h>

//SELF
#include "../Messages/Message.hpp"

namespace ASGE
{
	namespace KEYS
	{
		/**< Extra key defines */
		constexpr int KEY_F1 = 290;
		constexpr int KEY_F2 = 291;
		constexpr int KEY_F3 = 292;
		constexpr int KEY_F4 = 293;
		constexpr int KEY_LAST = 512; /**< GLFW defines it as 348 however I want to be safe. */
	}
}

/**
* Input Manager. Handles input thread safety and different key states.
*/

/*
The input manager has some issues but I'm leaving it as it is for the time being.
Use isKeyDown over isKeyPressed if at all possible.
*/
class InputManager
{
public:
	InputManager(ASGE::Input* input) noexcept;
	~InputManager();

	void update();
	void handleInput(int key, int state);

	void addAction(HashedID action, unsigned id);
	bool removeAction(HashedID action, unsigned id);

	bool isActionPressed(HashedID action);
	bool isActionDown(HashedID action);

	bool isKeyPressed(int key);
	bool isKeyDown(int key) noexcept;

	bool isGamePadButtonPressed(int button);
	bool isGamePadButtonDown(int button);

	bool isMouseButtonPressed(int button);
	bool isMouseButtonReleased(int button);
	bool isMouseButtonDown(int button);

	void getMouseScreenPosition(double& xpos, double& ypos);
	void getMouseWorldPosition(double& xpos, double& ypos);

	void applyOffset(float x, float y);

	GamePadData getGamePad();

	int gamepad_button_up = ASGE::KEYS::KEY_LAST + 10;
	int gamepad_button_down = ASGE::KEYS::KEY_LAST + 12;
	int gamepad_button_enter = ASGE::KEYS::KEY_LAST + 0;
	int gamepad_button_escape = ASGE::KEYS::KEY_LAST + 7;

private:
	void gamepadHandler(const ASGE::SharedEventData data);
	void mouseHandler(const ASGE::SharedEventData data);
	void keyHandler(const ASGE::SharedEventData data);

	int gamepad_id;
	bool gamepad_connected = false;
	int callback_id;
	int callback_id2;
	int callback_id3;

	ASGE::Input* input;
	std::mutex keys_mutex;
	std::mutex mouse_mutex;
	
	std::multimap<HashedID, int> actions;

	std::array<int, ASGE::KEYS::KEY_LAST> toggle_keys;
	std::array<int, ASGE::KEYS::KEY_LAST> keys;

	//todo: try and remember why I doubled this :( related to both gamepad buttons and keys having consecutive ranges
	std::array<int, ASGE::KEYS::KEY_LAST + ASGE::KEYS::KEY_LAST> gamepad_buttons_last_frame;
	std::array<int, ASGE::KEYS::KEY_LAST + ASGE::KEYS::KEY_LAST> gamepad_buttons;

	std::array<int, 512> mouse_buttons_last_frame;
	std::array<int, 512> mouse_buttons;
	std::array<int, 512> mouse_buttons_this_frame;

	float offset_x = 0;
	float offset_y = 0;
};
