#pragma once

//STD
#include <functional>
#include <string>

class Dialogue
{
public:
	using FunctionType = std::function<std::string()>;

	Dialogue(std::string dialogue_name, std::string speaker_name,
		FunctionType dialogue_text, FunctionType next_dialogue,
		bool player_option = false);

	const std::string name;
	const std::string speaker;
	const bool player_option;

	const FunctionType text;
	const FunctionType next;
};