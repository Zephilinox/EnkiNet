﻿
struct vector2
{
	float x = 0;
	float y = 0;

	friend Packet& operator <<(Packet& p, const vector2& v);
	friend Packet& operator >>(Packet& p, vector2& v);
};

Packet& operator <<(Packet& p, const vector2& v)
{
	return p << v.x << v.y;
}

Packet& operator >>(Packet& p, vector2& v)
{
	return p >> v.x >> v.y;
}

TEST_CASE("Packet")
{
	SUBCASE("Strings")
	{
		Packet p;
		std::string s = "hey";
		p << s;
		std::string s2 = ".";
		p >> s2;
		CHECK(s2 == s);
	}

	SUBCASE("Custom Class")
	{
		Packet p;
		vector2 v{ 1, 3 };
		p << v;
		vector2 v2;
		p >> v2;
		CHECK(v2.x == v.x);
		CHECK(v2.y == v.y);
	}

	SUBCASE("Vector")
	{
		Packet p;
		std::vector<int> vecs{ 1, 2 };
		p << vecs;
		std::vector<int> vecs2{};
		p >> vecs2;
		CHECK(vecs2 == vecs);
	}

	SUBCASE("Array")
	{
		Packet p;
		std::array<bool, 10> bools = { true, true, false, true, true, true, true, true, true, true };
		p << bools;
		std::array<bool, 10> bools2;
		p >> bools2;
		CHECK(bools2 == bools);
	}

	SUBCASE("Header")
	{
		Packet p;
		int oldtype = p.get_header().type;
		int oldtype_bytes = static_cast<int>(p.get_bytes().data()[0]);
		CHECK(static_cast<int>(oldtype) == oldtype_bytes);

		p.set_header({ PacketType::GLOBAL_RPC });

		int type = p.get_header().type;
		int type_bytes = static_cast<int>(p.get_bytes().data()[0]);
		CHECK(static_cast<int>(type) == type_bytes);
	}

	SUBCASE("Write Bits")
	{
		//todo: remove new packet to check for accurate byte boundary changes
		Packet p2;
		int num = 0b00001111;
		p2.write_bits(num, 8);
		int resulting_num = static_cast<int>(p2.get_bytes().data()[2]);
		CHECK(num == resulting_num);

		int num2 = 0b01001100;
		p2.write_bits(num2, 8);
		int resulting_num2 = static_cast<int>(p2.get_bytes().data()[3]);
		CHECK(num2 == resulting_num2);

		int num3 = 0b00001111;
		p2.write_bits(num3, 4);
		int resulting_num3 = static_cast<int>(p2.get_bytes().data()[4]);
		CHECK(0b00001111 == resulting_num3);

		int num4 = 0b11110000;
		p2.write_bits(num4, 4, 4);
		int resulting_num4 = static_cast<int>(p2.get_bytes().data()[4]);
		CHECK(0b11111111 == resulting_num4);
	}

	SUBCASE("Read Bits")
	{
		Packet p2;
		p2 << 0b00001111;
		int input = static_cast<int>(p2.get_bytes().data()[2]);
		int output = 0b00000000;
		p2.read_bits(output, 8);
		CHECK(input == output);

		p2 << 0b11110000;
		int input2 = static_cast<int>(p2.get_bytes().data()[3]);
		int output2 = 0b00000000;
		p2.read_bits(output2, 8);
		CHECK(input2 == output2);

		Packet p3;
		int num = 0b11111111;
		p3.write_bits(num, 8);
		int output3 = 0;
		int output4 = 0;
		p3.read_bits(output3, 4);
		p3.read_bits(output4, 4, 4);
		CHECK(output3 == 0b00001111);
		CHECK(output4 == 0b11110000);
		CHECK(output3 + output4 == num);
	}

	SUBCASE("Write Bits Overflowing Bytes")
	{
		Packet p;
		try
		{
			int num = 0b000001111;
			p.write_bits(num, 4);
			p.write_bits(num, 4);

			p.write_bits(num, 6);
			p.write_bits(num, 4);

			p.write_bits(num, 6);
		}
		catch (...)
		{

		}

		CHECK(p.get_bytes().size() == sizeof(PacketHeader) + 3);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader)]) == 0b11111111);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader) + 1]) == 0b11001111);
		CHECK(static_cast<int>(p.get_bytes()[sizeof(PacketHeader) + 2]) == 0b00111111);
	}

	SUBCASE("Read Bits Overflowing Bytes")
	{
		Packet p;
		int num = 0;
		p << 1000000;
		p.read_bits(num, 32);
		CHECK(num == 1000000);
	}

	SUBCASE("Compressed Range")
	{
		Packet p;
		float f1 = 0.5f;
		p.write_compressed_float(f1, 0, 1, 0.01f);
		float f2;
		p.read_compressed_float(f2, 0, 1, 0.01f);
		CHECK(f1 == f2);

		//will fail due to not supporting > 8 bits on read/write
		/*Packet p2;
		float f3 = 5;
		p2.write_compressed_float(f3, -10, 10, 0.01f);
		float f4;
		p2.read_compressed_float(f4, -10, 10, 0.01f);
		CHECK(f3 == f4);

		Packet p3;
		float f5 = 1.0f / 60.0f;
		p3.write_compressed_float(f5, 0, 1, 0.001f);
		float f6;
		p3.read_compressed_float(f6, 0, 1, 0.001f);
		CHECK(f5 == f6);*/
	}

	SUBCASE("Real Test")
	{
		Packet p({ PacketType::ENTITY });
		std::array<float, 2> position = { 300, 400 };
		float rotation = 27.27f;
		bool isplayer = true;
		int isplayer_int = static_cast<int>(isplayer);

		p.write_compressed_float(position[0], 0, 1280, 0.01f);
		p.write_compressed_float(position[1], 0, 720, 0.01f);
		p.write_compressed_float(rotation, 0, 360, 0.01f);
		p.write_bits(isplayer_int, 1); //todo: write bits not just for ints

		std::array<float, 2> received_pos;
		float received_rot;
		int received_isplayer;
		bool actual_isplayer;

		p.read_compressed_float(received_pos[0], 0, 1280, 0.01f);
		p.read_compressed_float(received_pos[1], 0, 720, 0.01f);
		p.read_compressed_float(received_rot, 0, 360, 0.01f);
		p.read_bits(received_isplayer, 1);
		actual_isplayer = static_cast<bool>(received_isplayer);

		CHECK(p.get_bytes().size() < sizeof(PacketHeader) + sizeof(position) + sizeof(rotation) + sizeof(isplayer));
		std::cout << p.get_bytes().size() << " < " << sizeof(PacketHeader) + sizeof(position) + sizeof(rotation) + sizeof(isplayer) << ", yay bytes saved!\n";
		CHECK(position[0] == doctest::Approx(received_pos[0]));
		CHECK(position[1] == doctest::Approx(received_pos[1]));
		CHECK(rotation == received_rot);
		CHECK(isplayer == actual_isplayer);
	}
}