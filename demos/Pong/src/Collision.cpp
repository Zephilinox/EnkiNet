#include "Collision.hpp"

//LIBS
#include <spdlog/fmt/fmt.h>
#include <EnkiNet/Scenegraph.hpp>

//SELF
#include "Ball.hpp"
#include "Paddle.hpp"

Collision::Collision(enki::EntityInfo info, enki::GameData* game_data)
	: Entity(info, game_data)
{
}

void Collision::onSpawn()
{
	ball_collider.height = 32;
	ball_collider.width = 32;

	paddle1_collider.height = 128;
	paddle1_collider.width = 32;

	paddle2_collider.height = 128;
	paddle2_collider.width = 32;
}

void Collision::update([[maybe_unused]]float dt)
{
	auto console = spdlog::get("console");
	auto scenegraph = game_data->scenegraph;

	Ball* ball = nullptr;
	Paddle* paddle1 = nullptr;
	Paddle* paddle2 = nullptr;

	{
		auto balls = scenegraph->findEntitiesByType("Ball");
		if (!balls.empty())
		{
			ball = static_cast<Ball*>(balls[0]);
		}
	}

	{
		auto paddle1s = scenegraph->findEntitiesByName("Paddle 1");
		if (!paddle1s.empty())
		{
			paddle1 = static_cast<Paddle*>(paddle1s[0]);
		}
	}

	{
		auto paddle2s = scenegraph->findEntitiesByName("Paddle 2");
		if (!paddle2s.empty())
		{
			paddle2 = static_cast<Paddle*>(paddle2s[0]);
		}
	}

	if (ball && paddle1 && paddle2)
	{
		ball_collider.left = ball->sprite.getPosition().x;
		ball_collider.top = ball->sprite.getPosition().y;
		
		{
			paddle1_collider.left = paddle1->sprite.getPosition().x;
			paddle1_collider.top = paddle1->sprite.getPosition().y;

			if (ball_collider.intersects(paddle1_collider))
			{
				ball->moving_left = false;

				if (ball->y_dir == 0)
				{
					if (std::rand() % 2)
					{
						ball->y_dir = 1;
					}
					else
					{
						ball->y_dir = -1;
					}
				}
			}
		}
		
		{
			paddle2_collider.left = paddle2->sprite.getPosition().x;
			paddle2_collider.top = paddle2->sprite.getPosition().y;

			if (ball_collider.intersects(paddle2_collider))
			{
				ball->moving_left = true;

				if (ball->y_dir == 0)
				{
					if (std::rand() % 2)
					{
						ball->y_dir = 1;
					}
					else
					{
						ball->y_dir = -1;
					}
				}
			}
		}
	}	
}