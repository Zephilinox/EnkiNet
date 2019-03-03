#include "DialogueTree.hpp"

//STD
#include <assert.h>
#include <locale>

//SELF
#include "Actor.hpp"

//todo: fix strings througout dialogue system to be performant (minor issue)

const std::string DialogueTree::player = "player";

void DialogueTree::addDialogue(std::string dialogue_name, std::string speaker_name, Dialogue::FunctionType dialogue_text, Dialogue::FunctionType next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), std::move(speaker_name), dialogue_text, next_dialogue);
}

void DialogueTree::addDialogue(std::string dialogue_name, std::string speaker_name, std::string dialogue_text, Dialogue::FunctionType next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), std::move(speaker_name), [dialogue_text](){return dialogue_text;}, next_dialogue);
}

void DialogueTree::addDialogue(std::string dialogue_name, std::string speaker_name, Dialogue::FunctionType dialogue_text, std::string next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), std::move(speaker_name), dialogue_text, [next_dialogue](){return next_dialogue;});
}

void DialogueTree::addDialogue(std::string dialogue_name, std::string speaker_name, std::string dialogue_text, std::string next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), std::move(speaker_name), [dialogue_text](){return dialogue_text;}, [next_dialogue](){return next_dialogue;});
}

void DialogueTree::addPlayerOption(std::string dialogue_name, Dialogue::FunctionType dialogue_text, Dialogue::FunctionType next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), player, dialogue_text, next_dialogue, true);
}

void DialogueTree::addPlayerOption(std::string dialogue_name, std::string dialogue_text, Dialogue::FunctionType next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), player, [dialogue_text]() {return dialogue_text; }, next_dialogue, true);
}

void DialogueTree::addPlayerOption(std::string dialogue_name, Dialogue::FunctionType dialogue_text, std::string next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), player, dialogue_text, [next_dialogue]() {return next_dialogue; }, true);
}

void DialogueTree::addPlayerOption(std::string dialogue_name, std::string dialogue_text, std::string next_dialogue)
{
	dialogues.emplace_back(std::move(dialogue_name), player, [dialogue_text]() {return dialogue_text; }, [next_dialogue]() {return next_dialogue; }, true);
}

Actor* DialogueTree::getActor(std::string name)
{
	for (size_t i = 0; i < actors.size(); ++i)
	{
		if (actors[i].name == name)
		{
			return &actors[i];
		}
	}

	actors.emplace_back(name);
	actors.back().realName = formatActorName(name);
	return &actors.back();
}

Actor* DialogueTree::getPlayer()
{
	return getActor(player);
}

Actor* DialogueTree::getSpeaker()
{
	if (player_option || !current_dialogue)
	{
		return nullptr;
	}

	return getActor(current_dialogue->speaker);
}

Actor* DialogueTree::getPreviousSpeaker() noexcept
{
	return previous_speaker;
}

Dialogue* DialogueTree::getCurrentDialogue() noexcept
{
	return current_dialogue;
}

Dialogue* DialogueTree::getPreviousDialogue() noexcept
{
	return previous_dialogue;
}

std::string DialogueTree::play(std::string dialogue_name)
{
	//Don't ask how this works. it's a mess.

	//passed empty dialog string, stop
	if (dialogue_name == "")
	{
		previous_speaker = nullptr;
		previous_dialogue = nullptr;
		current_dialogue = nullptr;
		playing = false;
		return "";
	}

	//if this is true before we play the next dialogue, reset it to false (last dialogue was a player option)
	if (player_option)
	{
		current_player_options.clear();
		player_option = false;
	}

	//find the dialogue we're meant to be playing
	for (auto& d : dialogues)
	{
		if (d.name == dialogue_name)
		{
			if (!player_option && !d.player_option)
			{
				//we haven't found a player option yet and this isn't a player option
				//so we start this dialogue, run text lambda, and return the result
				if (current_dialogue)
				{
					previous_speaker = getActor(current_dialogue->speaker);
					previous_dialogue = current_dialogue;
				}

				current_dialogue = &d;
				playing = true;
				std::cout << "RUNNING DIALOGUE TEXT DETERMINATOR FOR '" << d.name << "'\n";
				std::string text = d.text();
				std::cout << "text = " << text << "\n";
				if (text != "")
				{
					return std::move(text);
				}
				else
				{
					//uhoh, normal dialogue should never return a blank screen, dialogue is malformed
					std::cout << "ERROR: DIALOGUE TEXT FOR '" << d.name << "' IS EMPTY. STOPPING.\n";
					previous_dialogue = current_dialogue;
					current_dialogue = nullptr;
					playing = false;
					return "";
				}
			}
			else
			{
				//found a player option, push it to the vector and keep looping to find more options with the same name
				if (current_dialogue)
				{
					previous_speaker = getActor(current_dialogue->speaker);
					previous_dialogue = current_dialogue;
				}
				
				current_dialogue = nullptr;
				playing = true;
				player_option = true;
				current_player_options.push_back(&d);
			}
		}
	}

	if (player_option)
	{
		//called a player_dialogue so there's no specific text to return, return empty
		//could be more than 1 player dialogue, so no way to return them all
		//use current_player_options to get them all
		return "";
	}
	else
	{
		//if we got to this point it means nothing was found, so reset.
		previous_speaker = nullptr;
		previous_dialogue = current_dialogue;
		current_dialogue = nullptr;
		playing = false;
		return "";
	}
}

std::string DialogueTree::next()
{
	if (current_dialogue)
	{
		std::cout << "\nRUNNING NEXT DIALOGUE DETERMINATOR FOR '" << current_dialogue->name << "'\n";
		std::string n = current_dialogue->next();
		std::cout << "GOING TO NEXT DIALOGUE: '" << n << "'\n";
		return play(n);
	}

	//something seriously wrong has happened, I have no idea what. You get one poop emoji.
	playing = false;
	return "...\n";
}

std::string DialogueTree::formatActorName(std::string name)
{	
	name[0] = std::toupper(name[0], std::locale());

	for (size_t i = 0; i < name.length(); ++i)
	{
		if (name[i] == '_')
		{
			name[i] = ' ';

			if (i < name.length() - 1)
			{
				name[i + 1] = std::toupper(name[i + 1], std::locale());
			}
		}
	}

	return name;
}
