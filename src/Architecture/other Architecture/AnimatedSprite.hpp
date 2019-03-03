#pragma once

//STD
#include <vector>
#include <memory>
#include <chrono>

//LIB
#include <Engine/Sprite.h>

//todo: utilise timer class
typedef std::chrono::high_resolution_clock Clock;

class AnimatedSprite
{
public:
	struct FrameData
	{
		float relative_x;
		float relative_y;
		float frame_length_seconds;
	};

	AnimatedSprite(ASGE::Renderer* renderer, bool loop = true) noexcept;

	AnimatedSprite(const AnimatedSprite& other) = delete;
	AnimatedSprite(AnimatedSprite&& other) = delete;
	AnimatedSprite& operator= (const AnimatedSprite& other) = delete;
	AnimatedSprite& operator= (AnimatedSprite&& other) = delete;
	void update(double dt);

	void addFrame(std::string texture, float frame_length_seconds, float relative_x = 0, float relative_y = 0, float width_mult = 1, float height_mult = 1);
	ASGE::Sprite* getCurrentFrameSprite() const;

	void play() noexcept;
	void pause() noexcept;
	void restart() noexcept;
	bool isOver() noexcept;

	float xPos = 0;
	float yPos = 0;

private:
	void nextFrame();

	ASGE::Renderer* renderer;

	bool playing = true;
	bool loop;

	std::vector<std::pair<std::unique_ptr<ASGE::Sprite>, FrameData>> frames;
	size_t current_frame = 0;
	double frame_time = 0;
};