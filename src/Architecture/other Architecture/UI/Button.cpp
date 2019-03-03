#include "Button.hpp"

//SELF
#include "../GameData.hpp"
#include "../Managers/FontManager.hpp"
#include "../Constants.hpp"

Button::Button(ASGE::Renderer* renderer) noexcept
	: sprite(renderer->createUniqueSprite())
{
}

Button::Button(Button&& button) noexcept
	: sprite(std::move(button.sprite))
{
	selected = button.selected;
	pos_x = button.pos_x;
	pos_y = button.pos_y;
	name = std::move(button.name);
	colour = button.colour;
	selected_colour = button.selected_colour;
	on_click = std::move(button.on_click);
}

void Button::render(GameData* game_data, int z_order) const
{
	if (sprite->getTexture())
	{
		game_data->getRenderer()->renderSprite(*sprite, (float)z_order - 1);
	}

	game_data->getFontManager()->setFont("Dialogue");

	if (selected)
	{
		game_data->getRenderer()->renderText(name.c_str(), (int)pos_x, (int)pos_y, 1.0f, selected_colour, (float)z_order);
	}
	else
	{
		game_data->getRenderer()->renderText(name.c_str(), (int)pos_x, (int)pos_y, 1.0f, colour, (float)z_order);
	}
}

void Button::loadTexture(std::string texture)
{
	if (!sprite->loadTexture(texture))
	{
		throw;
	}

	sprite->xPos(pos_x - 1.0f);
	sprite->yPos(pos_y - 16.0f);
}

bool Button::isSelected() const noexcept
{
	return selected;
}

void Button::setSelected(bool s) noexcept
{
	selected = s;
}

bool Button::isEnabled() const
{
	return enabled;
}

void Button::setEnabled(bool new_enabled)
{
	enabled = new_enabled;
}

float Button::getPosX() const noexcept
{
	return pos_x;
}

float Button::getPosY() const noexcept
{
	return pos_y;
}

ASGE::Sprite* Button::getSprite()
{
	return sprite.get();
}

void Button::setPos(float x, float y) noexcept
{
	pos_x = x;
	pos_y = y;

	if (sprite)
	{
		sprite->xPos(pos_x - 3.0f);
		sprite->yPos(pos_y - 16.0f);
	}
}

void Button::setName(std::string n)
{
	name = n;
}

void Button::setColour(ASGE::Colour c) noexcept
{
	colour = c;
}

void Button::setSelectedColour(ASGE::Colour c) noexcept
{
	selected_colour = c;
}

void Button::setSize(float width, float height)
{
	sprite->width(width);
	sprite->height(height);
}

void Button::setSize(std::tuple<float, float> size)
{
	auto w = std::get<0>(size);
	auto h = std::get<0>(size);
	sprite->width(w);
	sprite->height(h);
}

std::tuple<float, float> Button::getSize()
{
	return std::tuple<float, float>(sprite->width(), sprite->height());
}
