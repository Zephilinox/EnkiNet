#pragma once

//LIBS
#include <SFML/Graphics.hpp>
#include <EnkiNet/Entity.hpp>

class Score : public enki::Entity
{
public:
	Score(enki::EntityInfo info, enki::GameData* game_data);

	void onSpawn() final;
	void draw(sf::RenderWindow& window) const final;

	void increaseScore1();
	void increaseScore2();

private:
	sf::Font font;
	sf::Text score1;
	sf::Text score2;

	int score1_points = 0;
	int score2_points = 0;
};