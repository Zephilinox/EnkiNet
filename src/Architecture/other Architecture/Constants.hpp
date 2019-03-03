#pragma once

/*! \file Constants.h
@brief   Constants that are used throughout the game. 
@details Add any new constants to this file. 
*/

constexpr float PI = 3.14159f;
constexpr float ANGLE_UP = 0;
constexpr float ANGLE_RIGHT = PI / 2;
constexpr float ANGLE_DOWN = PI;
constexpr float ANGLE_LEFT = -PI / 2;
constexpr float dark[3] = { 0.4f, 0.349f, 0.317f};

enum Z_ORDER_LAYER
{
	BACK_GROUND = 0,
	MIDDLE_GROUND = 1000,
	FORE_GROUND = 2000,
	UNITS = 3000,
	OVERLAY = 5000,
	OVERLAY_TEXT = 6000
};

struct MoveData
{
	float x = 0;
	float y = 0;
	int time_units = 0;
};