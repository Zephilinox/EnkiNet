#include "Dialogue.hpp"

Dialogue::Dialogue(std::string dialogue_name, std::string speaker_name, FunctionType dialogue_text, FunctionType next_dialogue, bool player_option)
	: name(dialogue_name)
	, speaker(speaker_name)
	, text(dialogue_text)
	, next(next_dialogue)
	, player_option(player_option)
{};