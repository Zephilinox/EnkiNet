#pragma once

//STD
#include <memory>

//LIB
#include <Engine/GameTime.h>

class GameData;

class State
{
public:
	State() = delete;
	virtual ~State() noexcept = default;

	/**
	*   @brief   Processes game logic
	*   @return  void
	*/
	virtual void update(const ASGE::GameTime&) = 0;


	/**
	*   @brief   Draw's stuff to the screen
	*   @details Called after the screen is cleared, and before it is shown.
	*   @return  void
	*/
	virtual void render() const = 0;


	/**
	*   @brief   Called when the state becomes active
	*   @details Called once by StateManager when this state is on top
	*   @return  void
	*/
	virtual void onActive() = 0;

	/**
	*   @brief   Called when the state becomes inactive
	*   @details Called once by StateManager when a different state is on top
	*   @return  void
	*/
	virtual void onInactive() = 0;

	bool shouldRenderPreviousState() noexcept
	{
		return render_previous_state;
	}
	
	bool shouldUpdatePreviousState() noexcept
	{
		return update_previous_state;
	}

protected:
	State(GameData* game_data, bool render_previous_state = false, bool update_previous_state = false)
		: game_data(game_data)
		, render_previous_state(render_previous_state)
		, update_previous_state(update_previous_state)
	{}

	GameData* game_data;

	bool render_previous_state = false;
	bool update_previous_state = false;
};