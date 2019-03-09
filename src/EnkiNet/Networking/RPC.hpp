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
struct rpc;

template <typename Return, typename... Parameters>
struct rpc<Return(Parameters...)>
{
	using return_t = Return;

	template <typename F>
	std::function<void(Packet)> wrap(F f)
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
struct rpc<Return((Class::*)(Parameters...))>
{
	using return_t = Return;

	template <typename F>
	std::function<void(Packet, Class*)> wrap(F f)
	{
		return [f](Packet p, Class* instance)
		{
			(instance->*f)(p.read<Parameters>()...);
		};
	}

	template <typename F>
	std::function<void(Packet, Entity*)> wrapEntity(F f)
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
		static_assert(std::is_void<typename rpc<F>::return_t>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (functions.count(name))
		{
			return;
		}

		rpc<F> rpc;
		functions[name] = rpc.wrap(func);
		std::cout << "rpc " << name << " registered\n";
	}

	//Register a member RPC with a member function
	template <typename R, typename Class, typename... Args>
	void add(std::string name, R(Class::*func)(Args...))
	{
		static_assert(std::is_void<R>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (RPCWrapper<Class>::functions.count(name))
		{
			return;
		}

		rpc<R(Class::*)(Args...)> rpc;
		RPCWrapper<Class>::functions[name] = rpc.wrap(func);
		std::cout << "rpc " << name << " registered\n";
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

		rpc<R(Class::*)(Args...)> rpc;
		entity_functions[type][name] = rpc.wrapEntity(func);
		std::cout << "rpc " << name << " registered for " + type + "\n";
	}

	//Serialize variadic template args to packet in reverse (now correct) order, so as to fix right-to-left ordering
	void rpcPacket()
	{

	}

	template <typename T>
	void rpcPacket(Packet& p, T x)
	{
		p << x;
	}

	template <typename T, typename... Args>
	void rpcPacket(Packet& p, T x, Args... args)
	{
		rpcPacket(p, args...);
		p << x;
	}

	template <typename... Args>
	Packet rpcPacket(Args... args)
	{
		Packet p;
		rpcPacket(p, args...);
		return p;
	}

	//Handle a global RPC packet
	void receive(Packet p)
	{
		try
		{
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

			//std::cout << "received packet to call rpc " << name << "\n";
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
			if (p.getBytes().size() <= p.getBytesRead())
			{
				std::cout << "Invalid RPC packet received due to being empty, ignoring\n";
				return;
			}

			auto info = p.read<EntityInfo>();
			auto name = p.read<std::string>();

			if constexpr(std::is_same_v<T, Entity>)
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

				//std::cout << "received packet to call rpc " << name << "\n";
				RPCWrapper<T>::functions[name](p, instance);
			}
		}
		catch (std::exception&)
		{
			std::cout << "Invalid RPC packet received that threw an exception, ignoring\n";
		}
	}

	//call local global rpc
	template <typename F, typename... Args>
	void call([[maybe_unused]] F* f, std::string name, Args... args)
	{
		if (functions.count(name))
		{
			//std::cout << "safe call to rpc " << name << " with the values";
			//((std::cout << " " << args), ...);
			//std::cout << "\n";
			static_assert(rpc<F>::template matchesArgs<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
			Packet p;

			//fill packet with rpc information
			p << name;
			rpcPacket(p, args...);

			//this call is only local
			receive(p);
		}
	}

	//call local class rpc
	template <typename R, typename Class, typename T, typename... Args>
	void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, T* instance, Args... args)
	{
		if (RPCWrapper<T>::functions.count(name))
		{
			//std::cout << "safe call to rpc " << name << " with the values";
			//((std::cout << " " << args), ...);
			//std::cout << "\n";
			static_assert(rpc<R(Class::*)(Args...)>::template matchesArgs<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
			Packet p({ PacketType::ENTITY_RPC });

			//fill packet with rpc information
			p << EntityInfo{};
			p << name;
			rpcPacket(p, args...);

			//this call is only local
			receive(p, instance);
		}
	}

	//call local and remote rpc
	template <typename R, typename Class, typename T, typename... Args>
	void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, NetworkManager* net_man, T* instance, Args... args)
	{
		if constexpr(std::is_base_of_v<Entity, T>)
		{
			auto type = static_cast<Entity*>(instance)->info.type;
			if (!entity_functions.count(type) || !entity_functions[type].count(name))
			{
				return;
			}
		}
		else
		{
			if (!RPCWrapper<T>::functions.count(name))
			{
				return;
			}
		}

		//todo, make this vary depending on if entity*, or just remove anyway

		//std::cout << "safe call to rpc " << name << " with the values";
		//((std::cout << " " << args), ...);
		//std::cout << "\n";
		static_assert(rpc<R(Class::*)(Args...)>::template matchesArgs<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
		Packet p({ PacketType::ENTITY_RPC });

		//fill packet with rpc information
		p << static_cast<Entity*>(instance)->info;
		p << name;
		rpcPacket(p, args...);

		if (net_man)
		{
			if (net_man->client)
			{
				net_man->client->sendPacket(0, &p);
			}
		}
	}

	//call local global rpc unsafe
	template <typename... Args>
	void callUnsafe(std::string name, Args... args)
	{
		if (functions.count(name))
		{
			//std::cout << "unsafe call to rpc " << name << " with the values";
			//((std::cout << " " << args), ...);
			//std::cout << "\n";
			Packet p;

			//fill packet with rpc information
			p << name;
			rpcPacket(p, args...);

			//this call is only local
			receive(p);
		}
	}

private:
	//Storage for all global rpc's
	std::map<std::string, std::function<void(Packet)>> functions;
	//Storage for all member function rpc's for derived from Entity classes
	std::map<std::string, std::map<std::string, std::function<void(Packet, Entity*)>>> entity_functions;
};
