#include <iostream>
#include <fstream>
#include <cassert>

#include "../include/json.h"

void test_types() {
	std::ifstream ifs {"test/tests/types.json"};
	auto json = JSON::parse(ifs);
	ifs.close();

	std::cout << json["label"]->str() << '\n';
	auto& numbers = json["content"]->object()["numbers"]->object();
	using JSON::Value;

	assert(numbers["real"]->type() == Value::Type::NUMBER);
	assert(numbers["real"]->number() == 3.14);
	assert(numbers["integer"]->type() == Value::Type::NUMBER);
	assert(numbers["integer"]->number() == 1337);

	auto& boolean = json["content"]->object()["boolean"]->object();
	assert(boolean["true"]->type() == Value::Type::BOOLEAN);
	assert(boolean["true"]->boolean());
	assert(boolean["false"]->type() == Value::Type::BOOLEAN);
	assert(!boolean["false"]->boolean());

	auto null = json["content"]->object()["null"]->nil();
	assert(null == nullptr);

	auto& str = json["content"]->object()["string"];
	assert(str->type() == Value::Type::STRING);
	assert(str->str() == "This is a string");

	auto& array = json["content"]->object()["array"];
	assert(array->type() == Value::Type::ARRAY);

	auto& object = json["content"]->object()["object"];
	assert(object->type() == Value::Type::OBJECT);

	std::cout << "All tests passed\n";
}

int main() {
	test_types();
	return EXIT_SUCCESS;
}
