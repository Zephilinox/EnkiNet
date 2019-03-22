#pragma once

//STD
#include <iostream>

//SELF
#include "Packet.hpp"
#include "Networking/ServerHost.hpp"
#include "Networking/ClientStandard.hpp"
#include "../Entity.hpp"

namespace enki
{
	enum RPCType
	{
		//RPC only runs for master
		//If you own the entity and call this RPC, it will execute only for you
		//If you don't own the entity and call this RPC, only the owner will execute the RPC
		Master,

		//RPC runs for all non-owners
		//If you own the entity and call this RPC, all remote entities will execute the RPC, but not yourself.
		//If you don't own the entity and call this RPC, nothing will happen
		Remote,

		//if you own the entity it will also execute it for you
		//Note that this might be slower than just using Remote and then calling the function directly as normal
		//This is provided mainly for ease-of-use, as well as when you might not be able to call it directly for some reason
		RemoteAndLocal,

		//RPC runs for masters and non-owners
		//Calling this RPC will execute it for everyone, except yourself
		MasterAndRemote,

		//Calling this RPC will execute it only for yourself
		Local,

		//Calling this RPC will execute it for everyone, including yourself
		//Note that this might be slower than just using MasterAndRemote and then calling the function directly as normal
		//This is provided mainly for ease-of-use, as well as when you might not be able to call it directly for some reason
		All,
	};

	struct EntityRPC
	{
		using FunctionType = std::function<void(Packet, Entity*)>;
		RPCType rpctype;
		FunctionType function;
	};

	struct GlobalRPC
	{
		using FunctionType = std::function<void(Packet)>;
		RPCType rpctype;
		FunctionType function;
	};
	
	inline Packet& operator <<(Packet& p, RPCType r)
	{
		p << static_cast<std::uint8_t>(r);
		return p;
	}

	inline Packet& operator >>(Packet& p, RPCType& r)
	{
		std::uint8_t k;
		p >> k;
		r = static_cast<RPCType>(k);
		return p;
	}

	//Used for storing member function RPC's for classes not derived from Entity*
	template <class Wrapee>
	class RPCWrapper
	{
	public:
		struct ClassRPC
		{
			using FunctionType = std::function<void(Packet, Wrapee*)>;
			RPCType rpctype;
			FunctionType function;
		};

		static std::map<std::string, ClassRPC> class_rpcs;
	};

	template <class Wrapee>
	std::map<std::string, typename RPCWrapper<Wrapee>::ClassRPC> RPCWrapper<Wrapee>::class_rpcs;

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
		void add(RPCType rpctype, std::string name, F* func)
		{
			static_assert(std::is_void<typename RPCUtil<F>::return_t>::value,
				"You can't register a function as an RPC if it doesn't return void");

			if (global_rpcs.count(name))
			{
				return;
			}

			global_rpcs[name].function = RPCUtil<F>::wrap(func);
			global_rpcs[name].rpctype = rpctype;
		}

		//Register a class RPC with a member function
		template <typename R, typename Class, typename... Args>
		void add(RPCType rpctype, std::string name, R(Class::*func)(Args...))
		{
			static_assert(std::is_void<R>::value,
				"You can't register a function as an RPC if it doesn't return void");

			if (RPCWrapper<Class>::class_rpcs.count(name))
			{
				return;
			}

			RPCWrapper<Class>::class_rpcs[name] = RPCUtil<R(Class::*)(Args...)>::wrap(func);
			RPCWrapper<Class>::rpctypes[name] = rpctype;
		}

		//Register a derived from Entity RPC with a member function
		template <typename R, typename Class, typename... Args>
		void add(RPCType rpctype, std::string type, std::string name, R(Class::*func)(Args...))
		{
			static_assert(std::is_void<R>::value,
				"You can't register a function as an RPC if it doesn't return void");

			if (entity_rpcs.count(type) && entity_rpcs[type].count(name))
			{
				return;
			}

			entity_rpcs[type][name].function = RPCUtil<R(Class::*)(Args...)>::wrapEntity(func);
			entity_rpcs[type][name].rpctype = rpctype;
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

				if (!global_rpcs.count(name))
				{
					std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
					return;
				}

				global_rpcs[name].function(p);
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

				if constexpr (std::is_base_of_v<Entity, T>)
				{
					if (!entity_rpcs.count(info.type) || !entity_rpcs[info.type].count(name))
					{
						std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
						return;
					}

					entity_rpcs[info.type][name].function(p, instance);
				}
				else
				{
					if (!RPCWrapper<T>::class_rpcs.count(name))
					{
						std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
						return;
					}

					RPCWrapper<T>::class_rpcs[name](p, instance);
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

			if (!global_rpcs.count(name))
			{
				return;
			}

			Packet p;
			p << name;
			fillPacket(p, args...);

			receive(p);
		}

		//call local class RPC
		template <typename R, typename Class, typename T, typename... Args>
		void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, T* instance, Args... args)
		{
			static_assert(RPCUtil<R(Class::*)(Args...)>::template matchesArgs<Args...>(),
				"You tried to call this rpc with the incorrect number or type of parameters");

			if (!RPCWrapper<T>::class_rpcs.count(name))
			{
				return;
			}

			Packet p({ PacketType::ENTITY_RPC });
			p << EntityInfo{} << name, ;
			fillPacket(p, args...);

			receive(p, instance);
		}

		//call local and remote RPC
		template <typename R, typename Class, typename T, typename... Args>
		void call([[maybe_unused]] R(Class::*f)(Args...), std::string name, NetworkManager* net_man, T* instance, Args... args)
		{
			static_assert(RPCUtil<R(Class::*)(Args...)>::template matchesArgs<Args...>(),
				"You tried to call this rpc with the incorrect number or type of parameters");

			if (instance == nullptr ||
				net_man == nullptr ||
				net_man->client == nullptr ||
				!net_man->client->isConnected())
			{
				return;
			}

			if constexpr (std::is_base_of_v<Entity, T>)
			{
				//We know it's derived from Entity, so it must have an info member variable, no need to cast it.
				if (!entity_rpcs.count(instance->info.type) ||
					!entity_rpcs[instance->info.type].count(name))
				{
					return;
				}

				bool local = false;
				bool networked = false;

				switch (entity_rpcs[instance->info.type][name].rpctype)
				{
					case Master:
						if (instance->isOwner())
						{
							local = true;
						}
						else
						{
							networked = true;
						}
						break;
					case Remote:
						if (instance->isOwner())
						{
							networked = true;
						}
						break;
					case RemoteAndLocal:
						if (instance->isOwner())
						{
							networked = true;
							local = true;
						}
						break;
					case MasterAndRemote:
						networked = true;
						break;
					case Local:
						local = true;
						break;
					case All:
						networked = true;
						local = true;
						break;
					default:
						break;
				}

				Packet p({ PacketType::ENTITY_RPC });
				p << instance->info << name;

				fillPacket(p, args...);

				if (networked)
				{
					net_man->client->sendPacket(0, &p);
				}

				if (local)
				{
					receive(p, instance);
				}
			}
			else
			{
				if (!RPCWrapper<T>::class_rpcs.count(name))
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
			if (!global_rpcs.count(name))
			{
				return
			}

			Packet p;
			p << name;
			fillPacket(p, args...);
			receive(p);
		}

		//call local entity RPC unsafe
		template <typename T, typename... Args>
		void callUnsafe(std::string name, T* instance, Args... args)
		{
			static_assert(std::is_base_of_v<Entity, T>);

			if (!entity_rpcs.count(instance->info.type) ||
				!entity_rpcs[instance->info.type].count(name))
			{
				return;
			}

			Packet p({ PacketType::ENTITY_RPC });
			p << instance->info;
			p << name;
			fillPacket(p, args...);
			receive(p, instance);
		}

		//todo: unsafe global remote
		//todo: unsafe entity local
		//todo: unsafe entity remote

		//global rpc
		RPCType getRPCType(std::string name)
		{
			return global_rpcs[name].rpctype;
		}

		//entity rpc
		RPCType getRPCType(std::string type, std::string name)
		{
			return entity_rpcs[type][name].rpctype;
		}

		//class rpc
		template <typename T>
		RPCType getRPCType(std::string name)
		{
			return RPCWrapper<T>::class_rpcs[name].rpctype;
		}

	private:
		//Serialize variadic template args to packet in reverse (now correct) order, so as to fix right-to-left ordering
		//not defined in packet header because this stuff is specific to parameter pack expansion and will get misused
		void fillPacket() {}
		void fillPacket([[maybe_unused]]Packet& p) {}

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
		std::map<std::string, GlobalRPC> global_rpcs;
		//Storage for all member function RPCUtil's for derived from Entity classes
		std::map<std::string, std::map<std::string, EntityRPC>> entity_rpcs;
	};
}