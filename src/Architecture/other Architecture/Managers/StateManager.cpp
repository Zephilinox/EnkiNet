#include "StateManager.hpp"

//STD
#include <iostream>

//SELF
#include "../State.hpp"
#include "../GameData.hpp"

StateManager::StateManager(GameData* game_data) noexcept
	: game_data(game_data)
{
}

void StateManager::update(const ASGE::GameTime& gt)
{
	if (states.empty())
	{
		return;
	}

	size_t firstUpdateState = 0;
	for (int i = states.size() - 1; i >= 0; --i)
	{
		if (!states[i]->shouldUpdatePreviousState())
		{
			firstUpdateState = i;
			break;
		}
	}

	for (size_t i = firstUpdateState; i < states.size(); ++i)
	{
		states[i]->update(gt);
	}
}

void StateManager::render() const
{
	if (states.empty())
	{
		return;
	}

	size_t firstRenderState = 0;
	for (int i = states.size() - 1; i >= 0; --i)
	{
		if (!states[i]->shouldRenderPreviousState())
		{
			firstRenderState = i;
			break;
		}
	}

	for (size_t i = firstRenderState; i < states.size(); ++i)
	{
		states[i]->render();
	}
}

State* StateManager::top() const
{
	if (states.empty())
	{
		throw std::runtime_error("StateManager::top - Stack is empty");
	}

	return states.back().get();
}

void StateManager::pop()
{
	game_data->getMessageQueue()->sendPriorityMessage<FunctionMessage>(
	[&]()
	{
		if (states.empty())
		{
			throw std::runtime_error("StateManager::pop - Stack is empty");
		}

		states.pop_back();

		if (!states.empty())
		{
			states.back()->onActive();
		}
	});
}

bool StateManager::empty() const noexcept
{
	return states.empty();
}
