#pragma once

//STD
#include <iostream>

//SELF
#include "Packet.hpp"
#include "Networking/ServerHost.hpp"
#include "Networking/ClientStandard.hpp"
#include "../Entity.hpp"

//Used for storing member function RPC's for classes not derived from Entity*
template <class Wrapee>
class RPCWrapper
{
public:
	static std::map<std::string, std::function<void(Packet, Wrapee*)>> functions;
};

template <class Wrapee>
std::map<std::string, std::function<void(Packet, Wrapee*)>> RPCWrapper<Wrapee>::functions;

//Used for getting type info from functions, and having that info available in the wrapped RPC functions
template <typename not_important>
struct RPCUtil;

template <typename Return, typename... Parameters>
struct RPCUtil<Return(Parameters...)>
{
	using return_t = Return;

	template <typename F>
	static std::function<void(Packet)> wrap(F f)
	{
		return [f](Packet p)
		{
			f(p.read<Parameters>()...);
		};
	}

	template <typename... Args>
	constexpr static bool matchesArgs()
	{
		return std::is_same_v<std::tuple<Parameters...>, std::tuple<Args...>>;
	}
};

template <typename Return, typename Class, typename... Parameters>
struct RPCUtil<Return((Class::*)(Parameters...))>
{
	using return_t = Return;

	template <typename F>
	static std::function<void(Packet, Class*)> wrap(F f)
	{
		return [f](Packet p, Class* instance)
		{
			(instance->*f)(p.read<Parameters>()...);
		};
	}

	template <typename F>
	static std::function<void(Packet, Entity*)> wrapEntity(F f)
	{
		static_assert(!std::is_same_v<Class, Entity>);
		
		return [f](Packet p, Entity* instance)
		{
			auto derived = static_cast<Class*>(instance);
			(derived->*f)(p.read<Parameters>()...);
		};
	}

	template <typename... Args>
	constexpr static bool matchesArgs()
	{
		return std::is_same_v<std::tuple<Parameters...>, std::tuple<Args...>>;
	}
};

class RPCManager
{
public:
	//Register a global RPC with a callable
	template <typename F>
	void add(std::string name, F* func)
	{
		static_assert(std::is_void<typename RPCUtil<F>::return_t>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (functions.count(name))
		{
			return;
		}

		functions[name] = RPCUtil<F>::wrap(func);
	}

	//Register a class RPC with a member function
	template <typename R, typename Class, typename... Args>
	void add(std::string name, R(Class::*func)(Args...))
	{
		static_assert(std::is_void<R>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (RPCWrapper<Class>::functions.count(name))
		{
			return;
		}

		RPCWrapper<Class>::functions[name] = RPCUtil<R(Class::*)(Args...)>::wrap(func);
	}

	//Register a derived from Entity RPC with a member function
	template <typename R, typename Class, typename... Args>
	void add(std::string type, std::string name, R(Class::*func)(Args...))
	{
		static_assert(std::is_void<R>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (entity_functions.count(type) && entity_functions[type].count(name))
		{
			return;
		}

		entity_functions[type][name] = RPCUtil<R(Class::*)(Args...)>::wrapEntity(func);
	}

	//Handle a global RPC packet
	void receive(Packet p)
	{
		try
		{
			//done this way because getBytesRead will be sizeof(PacketHeader), and anything less than that is invalid
			if (p.getBytes().size() <= p.getBytesRead())
			{
				std::cout << "Invalid RPC packet received due to being empty, ignoring\n";
				return;
			}

			std::string name = p.read<std::string>();

			if (!functions.count(name))
			{
				std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
				return;
			}

			functions[name](p);
		}
		catch (std::exception&)
		{
			std::cout << "Invalid RPC packet received that threw an exception, ignoring\n";
		}
	}

	//Handle a member function RPC packet
	template <typename T>
	void receive(Packet p, T* instance)
	{
		try
		{
			//done this way because getBytesRead will be sizeof(PacketHeader), and anything less than that is invalid
			if (p.getBytes().size() <= p.getBytesRead())
			{
				std::cout << "Invalid RPC packet received due to being empty, ignoring\n";
				return;
			}

			auto info = p.read<EntityInfo>();
			auto name = p.read<std::string>();

			if constexpr(std::is_base_of_v<Entity, T>)
			{
				if (!entity_functions.count(info.type) || !entity_functions[info.type].count(name))
				{
					std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
					return;
				}

				entity_functions[info.type][name](p, instance);
			}
			else
			{
				if (!RPCWrapper<T>::functions.count(name))
				{
					std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
					return;
				}

				RPCWrapper<T>::functions[name](p, instance);
			}
		}
		catch (std::exception&)
		{
			std::cout << "Invalid RPC packet received that threw an exception, ignoring\n";
		}
	}

	//call local global RPC
	template <typename F, typename... Args>
	void call([[maybe_unused]] F* f, std::string name, Args... args)
	{
		static_assert(RPCUtil<F>::template matchesArgs<Args...>(),
			"You tried to call this rpc with the incorrect number or type of parameters");

		if (functions.count(name))
		{
			Packet p;
			p << name;
			fillPacket(p, args...);

			receive(p);
		}
	}

	//call local class RPC
	template <typename R, typename Class, typename T, typename... Args>
	void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, T* instance, Args... args)
	{
		static_assert(RPCUtil<R(Class::*)(Args...)>::template matchesArgs<Args...>(),
			"You tried to call this rpc with the incorrect number or type of parameters");

		if (RPCWrapper<T>::functions.count(name))
		{
			Packet p({ PacketType::ENTITY_RPC });
			p << EntityInfo{} <<  name, ;
			fillPacket(p, args...);

			receive(p, instance);
		}
	}
	
	//call local and remote RPC
	template <typename R, typename Class, typename T, typename... Args>
	void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, NetworkManager* net_man, T* instance, Args... args)
	{
		static_assert(RPCUtil<R(Class::*)(Args...)>::template matchesArgs<Args...>(),
			"You tried to call this rpc with the incorrect number or type of parameters");

		if (instance == nullptr || net_man == nullptr || net_man->client == nullptr )
		{
			return;
		}

		if constexpr(std::is_base_of_v<Entity, T>)
		{
			//We know it's derived from Entity, so it must have an info member variable, no need to cast it.
			if (!entity_functions.count(instance->info.type) || !entity_functions[instance->info.type].count(name))
			{
				return;
			}

			Packet p({ PacketType::ENTITY_RPC });
			p << instance->info << name;
			fillPacket(p, args...);

			net_man->client->sendPacket(0, &p);

			receive(p, instance);
		}
		else
		{
			if (!RPCWrapper<T>::functions.count(name))
			{
				return;
			}

			Packet p({ PacketType::GLOBAL_RPC });
			p << name;
			fillPacket(p, args...);

			net_man->client->sendPacket(0, &p);
			receive(p, instance);
		}
	}

	//call local global RPC unsafe
	template <typename... Args>
	void callUnsafe(std::string name, Args... args)
	{
		if (functions.count(name))
		{
			Packet p;
			p << name;
			fillPacket(p, args...);
			receive(p);
		}
	}

	//call local entity RPC unsafe
	template <typename T, typename... Args>
	void callUnsafe(std::string name, T* instance, Args... args)
	{
		static_assert(std::is_base_of_v<Entity, T>);

		if (entity_functions.count(instance->info.type) && entity_functions[instance->info.type].count(name))
		{
			Packet p({ PacketType::ENTITY_RPC });
			p << instance->info;
			p << name;
			fillPacket(p, args...);
			receive(p, instance);
		}
	}

	//todo: unsafe global remote
	//todo: unsafe entity local
	//todo: unsafe entity remote

private:
	//Serialize variadic template args to packet in reverse (now correct) order, so as to fix right-to-left ordering
	//not defined in packet header because this stuff is specific to parameter pack expansion and will get misused
	void fillPacket() {}

	template <typename T>
	void fillPacket(Packet& p, T x)
	{
		p << x;
	}

	template <typename T, typename... Args>
	void fillPacket(Packet& p, T x, Args... args)
	{
		fillPacket(p, args...);
		p << x;
	}

	template <typename... Args>
	Packet fillPacket(Args... args)
	{
		Packet p;
		fillPacket(p, args...);
		return p;
	}

	//Storage for all global RPCUtil's
	std::map<std::string, std::function<void(Packet)>> functions;
	//Storage for all member function RPCUtil's for derived from Entity classes
	std::map<std::string, std::map<std::string, std::function<void(Packet, Entity*)>>> entity_functions;
};
