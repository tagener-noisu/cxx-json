#include <iostream>
#include <fstream>
#include <cassert>
#include "json.h"

void parse_stream(std::fstream& fs) {
	std::cout << "Parsing file.\n";
	auto o = JSON::parse(fs);
	std::cout << o << '\n';
}

void generate_json() {
	using JSON::Value;

	JSON::Object o = JSON::parse(R"({})");
	auto& t = o["string"];
	t = Value::make(std::string{"This is a string"});
	assert(t->type() == Value::Type::STRING);
	assert(t->str() == "This is a string");
}

void parse_literal() {
	using namespace JSON::Literals;
	auto o = R"({"what": "a flock of geese", "count": 1})"_object;
	auto a = R"(["India", "mittit", "ebur"])"_array;
	std::cout << a << '\n';
	std::cout << o << '\n';
}

int main(int argc, char** argv) {
	if (argc > 1) {
		std::fstream fs {argv[1], std::ios_base::in};
		parse_stream(fs);
	}

	//parse_literal();
	generate_json();
	std::cout << "All tests have been completed.\n";
}
