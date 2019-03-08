#pragma once

//STL
#include <memory>
#include <vector>
#include <functional>

//LIB
#include "Engine/GameTime.h"

//SELF
#include "../Messages/MessageQueue.hpp"
#include "../State.hpp"
#include "../Messages/FunctionMessage.hpp"

class GameData;

class StateManager
{
public:
	StateManager(GameData* game_data) noexcept;

	void update(const ASGE::GameTime& gt);
	void render() const;

	void pop();
	template <class T> void push();
	template <class T, class... Args> void push(Args... args);

	State* top() const;
	bool empty() const noexcept;

private:
	GameData* game_data;
	std::vector<std::unique_ptr<State>> states;
};

template <class T>
void StateManager::push()
{
	game_data->getMessageQueue()->sendPriorityMessage<FunctionMessage>(
	[&]()
	{
		if (!states.empty())
		{
			states.back()->onInactive();
		}

		states.push_back(std::make_unique<T>(game_data));
	});
}

template <class T, class... Args>
void StateManager::push(Args... args)
{
	game_data->getMessageQueue()->sendPriorityMessage<FunctionMessage>(
	[&, args...]()
	{
		if (!states.empty())
		{
			states.back()->onInactive();
		}

		states.push_back(std::make_unique<T>(game_data, args...));
	});
}