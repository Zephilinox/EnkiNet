#include "PlayerText.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>
#include <EnkiNet/Scenegraph.hpp>

//SELF
#include "Ball.hpp"
#include "Paddle.hpp"

PlayerText::PlayerText(EntityInfo info, GameData* game_data)
	: Entity(info, game_data)
{
}

void PlayerText::onSpawn()
{
	if (!font.loadFromFile("resources/arial.ttf"))
	{
		fmt::print("Failed to load resources/arial.ttf");
	}

	label.setFont(font);
	label.setString("Player " + std::to_string(info.ownerID));
	label.setFillColor(sf::Color::Black);
	label.setCharacterSize(12);
}

void PlayerText::input([[maybe_unused]]sf::Event& e)
{
}

void PlayerText::update([[maybe_unused]]float dt)
{
	auto console = spdlog::get("console");
	auto scenegraph = game_data->scenegraph;
	auto parent = scenegraph->getEntity(info.parentID);
	auto parent_paddle = static_cast<Paddle*>(parent);

	label.setPosition(parent_paddle->sprite.getPosition() - sf::Vector2f(6, 16));
}

void PlayerText::draw(sf::RenderWindow& window) const
{
	window.draw(label);
}

void PlayerText::serialize([[maybe_unused]]Packet& p)
{
	
}

void PlayerText::deserialize([[maybe_unused]]Packet& p)
{
	
}
