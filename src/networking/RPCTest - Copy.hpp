struct Entity
{
	void do_thing(int x, int y)
	{
		i = x + y;
		std::cout << "called function do_thing(" << x << ", " << y << ")\n";
	}

	int i;
	inline static std::map<std::string, std::function<void(Packet, Entity*)>> functions;
};

void one(int i, double d, float s, int ii)
{
	std::cout << "called function one(" << i << ", " << d << ", " << s << ", " << ii << ");\n";
}

TEST_CASE("RPC")
{
	RPCManager rpcm;
	rpcm.add("one", one);

	std::cout << "\n";

	rpcm.call(one, "one", 1, 2.0, 3.0f, 4);

	std::cout << "\n";

	rpcm.call_unsafe("one", 1.5, -2.0f, true, true);

	std::cout << "\n";

	//let's try and send a fake rpc
	rpcm.call_unsafe("two", true);
	//this checks the function is registered already, so it's fine

	std::cout << "\n";

	//let's do it manually
	Packet p;
	p << std::string("two") << true;
	rpcm.receive(p);
	//this makes the client throw an exception, so we need to capture that exception rather than crash the game, now fixed

	std::cout << "\n";

	//now let's try a valid rpc, but one which has the wrong types
	Packet p2;
	p2 << std::string("one") << true << true << true << true;
	rpcm.receive(p2);
	//this also causes an exception to be thrown and the client to crash, now fixed

	std::cout << "\n";

	//Now let's try sending one big arg to a valid function expecting 4 smaller args
	Packet p3;
	p3 << std::string("one") << 50 << 50;
	rpcm.receive(p3);

	SUBCASE("Entity")
	{
		Entity e;
		rpcm.add("do_thing", &Entity::do_thing);
		rpcm.call(&Entity::do_thing, "do_thing", &e, 1, 2);
		std::cout << e.i << "\n";
		REQUIRE(e.i == 3);
	}
}