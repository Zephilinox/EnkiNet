#pragma once

//STD
#include <functional>
#include <string>

//LIB
#include <Engine/Colours.h>
#include <Engine/Sprite.h>

//SELF
#include "../GameData.hpp"
#include "../Signals/Signal.hpp"
#include "../Constants.hpp"

class Button
{
public:
	Button(ASGE::Renderer* renderer) noexcept;
	Button(Button&& button) noexcept;

	void render(GameData* game_data, int z_order = Z_ORDER_LAYER::OVERLAY_TEXT) const;

	void loadTexture(std::string texture);

	bool isSelected() const noexcept;
	void setSelected(bool selected) noexcept;
	bool isEnabled() const;
	void setEnabled(bool enabled);

	float getPosX() const noexcept;
	float getPosY() const noexcept;
	ASGE::Sprite* getSprite();

	void setPos(float x, float y) noexcept;
	void setName(std::string name);
	void setColour(ASGE::Colour colour) noexcept;
	void setSelectedColour(ASGE::Colour colour) noexcept;
	void setSize(float width, float height);
	void setSize(std::tuple<float, float>);
	std::tuple<float, float> getSize();

	Signal<> on_click;

private:
	bool selected = false;
	bool enabled = true;
	
	float pos_x = 0;
	float pos_y = 0;
	std::string name = "Default";
	ASGE::Colour colour = ASGE::COLOURS::RED;
	ASGE::Colour selected_colour = ASGE::COLOURS::BLUE;
	std::unique_ptr<ASGE::Sprite> sprite;
};