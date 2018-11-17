#pragma once

class RPCManager
{
public:
	template <typename F>
	void add(std::string name, F* func)
	{
		static_assert(std::is_void<rpc<F>::return_t>::value,
			"You can't register a function as an RPC if it doesn't return void");

		if (functions.count(name))
		{
			return;
		}
		
		rpc<F> rpc;
		functions[name] = rpc.wrap(func);
		std::cout << "rpc " << name << " registered\n";
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

	void receive(Packet p)
	{
		try
		{
			if (p.get_bytes().size() <= p.get_bytes_read())
			{
				std::cout << "Invalid RPC packet received due to being empty, ignoring\n";
				return;
			}

			std::string name;
			p >> name;

			if (!functions.count(name))
			{
				std::cout << "Invalid RPC packet received due to invalid name, ignoring\n";
				return;
			}

			std::cout << "received packet to call rpc " << name << "\n";
			functions[name](p);
		}
		catch (std::exception&)
		{
			std::cout << "Invalid RPC packet received that threw an exception, ignoring\n";
		}
	}

	template <typename F, typename... Args>
	void call([[maybe_unused]] F* f, std::string name, Args... args)
	{
		if (functions.count(name))
		{
			std::cout << "safe call to rpc " << name << " with the values";
			((std::cout << " " << args), ...);
			std::cout << "\n";
			static_assert(rpc<F>::matches_arguments<Args...>(), "You tried to call this rpc with the incorrect number or type of parameters");
			Packet p;

			//fill packet with rpc information
			p << name;
			rpcPacket(p, args...);

			//simulate client received packet
			receive(p);
		}
	}

	template <typename... Args>
	void call_unsafe(std::string name, Args... args)
	{
		if (functions.count(name))
		{
			std::cout << "unsafe call to rpc " << name << " with the values";
			((std::cout << " " << args), ...);
			std::cout << "\n";
			Packet p;

			//fill packet with rpc information
			p << name;
			rpcPacket(p, args...);

			//simulate client received packet
			receive(p);
		}
	}

private:
	inline static std::map<std::string, std::function<void(Packet)>> functions;
};

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
	constexpr static bool matches_arguments()
	{
		return std::is_same_v<std::tuple<Parameters...>, std::tuple<Args...>>;
	}
};