#include "Collision.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>

//SELF
#include "../Architecture/Scenegraph.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

Collision::Collision(EntityInfo info, GameData* game_data)
	: Entity(info, game_data)
{
}

void Collision::onSpawn()
{
	fmt::print("ID={} name={} owner={} parent={} type={}\n", info.ID, info.name, info.ownerID, info.parentID, info.type);
	ball_collider.height = 32;
	ball_collider.width = 32;

	paddle1_collider.height = 128;
	paddle1_collider.width = 32;

	paddle2_collider.height = 128;
	paddle2_collider.width = 32;
}

void Collision::input(sf::Event& e)
{
}

void Collision::update(float dt)
{
	auto console = spdlog::get("console");
	auto scenegraph = game_data->scenegraph;
	if (scenegraph->entityExists(3))
	{
		auto ball = static_cast<Ball*>(scenegraph->getEntity(3));
		ball_collider.left = ball->sprite.getPosition().x;
		ball_collider.top = ball->sprite.getPosition().y;
		
		if (scenegraph->entityExists(1))
		{
			auto paddle1 = static_cast<Paddle*>(scenegraph->getEntity(1));
			paddle1_collider.left = paddle1->sprite.getPosition().x;
			paddle1_collider.top = paddle1->sprite.getPosition().y;
			
			if (ball_collider.intersects(paddle1_collider))
			{
				ball->moving_left = false;
				
				if (ball->y_dir == 0)
				{
					if (std::rand() % 2)
					{
						ball->y_dir == 1;
					}
					else
					{
						ball->y_dir == -1;
					}
				}
			}
		}
		
		if (scenegraph->entityExists(4))
		{
			auto paddle2 = static_cast<Paddle*>(scenegraph->getEntity(4));
			paddle2_collider.left = paddle2->sprite.getPosition().x;
			paddle2_collider.top = paddle2->sprite.getPosition().y;

			if (ball_collider.intersects(paddle2_collider))
			{
				ball->moving_left = true;

				if (ball->y_dir == 0)
				{
					if (std::rand() % 2)
					{
						ball->y_dir == 1;
					}
					else
					{
						ball->y_dir == -1;
					}
				}
			}
		}
	}	
}

void Collision::draw(sf::RenderWindow& window) const
{
	
}

void Collision::serialize(Packet& p)
{
	
}

void Collision::deserialize(Packet& p)
{
	
}
