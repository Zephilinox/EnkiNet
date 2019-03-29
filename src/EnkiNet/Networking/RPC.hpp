#pragma once

//SELF
#include "EnkiNet/Networking/Packet.hpp"
#include "EnkiNet/Networking/ServerHost.hpp"
#include "EnkiNet/Networking/ClientStandard.hpp"
#include "EnkiNet/Entity.hpp"

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
}