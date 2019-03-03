#pragma once

#include <cstdio>

//https://stackoverflow.com/questions/36117884/compile-time-typeid-for-every-type
//https://stackoverflow.com/questions/37658794/integer-constant-overflow-warning-in-constexpr
//https://stackoverflow.com/questions/45538778/constexpr-user-defined-literal-is-it-allowed
//https://gcc.godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(j:1,source:'%23include+%3Cstdio.h%3E%0A%0Aconstexpr+size_t+cx_hash(const+char*+input)+%7B%0A++++size_t+hash+%3D+sizeof(size_t)+%3D%3D+8+%3F+0xcbf29ce484222325+:+0x811c9dc5%3B%0A++++const+size_t+prime+%3D+sizeof(size_t)+%3D%3D+8+%3F+0x00000100000001b3+:+0x01000193%3B%0A%0A++++while+(*input)+%7B%0A++++++++hash+%5E%3D+static_cast%3Csize_t%3E(*input)%3B%0A++++++++hash+*%3D+prime%3B%0A++++++++%2B%2Binput%3B%0A++++%7D%0A%0A++++return+hash%3B%0A%7D%0A%0Aconstexpr+size_t+operator+%22%22_hash+(const+char+*input,+size_t)+%7B%0A++++return+cx_hash(input)%3B%0A%7D%0A%0Aint+main(void)+%7B%0A++++printf(%22%25i%5Cn%22,+%22test%22_hash)%3B%0A++++getchar()%3B%0A++++return+0%3B%0A%7D'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:50,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:g71,filters:(___0:(),b:'0',commentOnly:'0',directives:'0',intel:'0',jquery:'3.2.1',length:1,prevObject:(___0:(sizzle1502074381427:(undefined:(legend:!(16,0,'1')))),length:1,prevObject:(___0:(jQuery321055802776224425351:(display:''),sizzle1502074381427:(undefined:(legend:!(16,0,'1')))),length:1)),trim:'0'),options:'-O2',source:1),l:'5',n:'0',o:'x86-64+gcc+7.1+(Editor+%231,+Compiler+%231)',t:'0')),header:(),k:50,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4

using HashedID = uint32_t;

constexpr HashedID hash(const char* input) {
	HashedID hash = 0x811c9dc5;
	const HashedID prime = 0x01000193;

	while (*input) {
		hash ^= static_cast<HashedID>(*input);
		hash = static_cast<unsigned long long>(hash) * prime;
		++input;
	}

	return hash;
}

constexpr HashedID operator ""_hash(const char *input, uint32_t) {
	return hash(input);
}

class Message
{
public:
	Message() = default;
	virtual ~Message() noexcept = default;

	const HashedID id = ID;
	static constexpr HashedID ID = hash("Message");

protected:
	Message(HashedID id) noexcept
		: id(id)
	{}
};