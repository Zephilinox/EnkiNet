#pragma once

//STD
#include <functional>
#include <vector>
#include <unordered_set>
#include <string>
#include <experimental/vector>
#include <iomanip>

//SELF
#include "Dialogue.hpp"
#include "Actor.hpp"

class DialogueTree
{
public:
	static const std::string player;

	void addDialogue(std::string dialogue_name, std::string speaker_name, Dialogue::FunctionType dialogue_text, Dialogue::FunctionType next_dialogue);
	void addDialogue(std::string dialogue_name, std::string speaker_name, std::string dialogue_text, Dialogue::FunctionType next_dialogue);
	void addDialogue(std::string dialogue_name, std::string speaker_name, Dialogue::FunctionType dialogue_text, std::string next_dialogue);
	void addDialogue(std::string dialogue_name, std::string speaker_name, std::string dialogue_text, std::string next_dialogue);

	void addPlayerOption(std::string dialogue_name, Dialogue::FunctionType dialogue_text, Dialogue::FunctionType next_dialogue);
	void addPlayerOption(std::string dialogue_name,	std::string dialogue_text, Dialogue::FunctionType next_dialogue);
	void addPlayerOption(std::string dialogue_name,	Dialogue::FunctionType dialogue_text, std::string next_dialogue);
	void addPlayerOption(std::string dialogue_name,	std::string dialogue_text, std::string next_dialogue);

	Actor* getActor(std::string name);
	Actor* getPlayer();
	Actor* getSpeaker();
	Actor* getPreviousSpeaker() noexcept;
	Dialogue* getCurrentDialogue() noexcept;
	Dialogue* getPreviousDialogue() noexcept;

	std::string play(std::string dialogue_name);
	std::string next();

	bool playing = false;
	bool player_option = false;
	std::vector<Dialogue*> current_player_options;

private:
	std::string formatActorName(std::string name);

	std::vector<Dialogue> dialogues;
	std::vector<Actor> actors;
	Actor* previous_speaker;
	Dialogue* current_dialogue = nullptr;
	Dialogue* previous_dialogue = nullptr;
};