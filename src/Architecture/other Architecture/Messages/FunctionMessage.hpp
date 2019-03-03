#pragma once

//STD
#include <functional>

//SELF
#include "Message.hpp"

class FunctionMessage : public Message
{
public:
	FunctionMessage(std::function<void()> func)
		: Message(ID)
		, function(func)
	{}

	void execute()
	{
		function();
	}

	static constexpr HashedID ID = hash("Function");

private:
	std::function<void()> function;
};