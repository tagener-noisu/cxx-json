#include <iostream>
#include <fstream>
#include <cassert>

#include "../include/json.h"

void test_creation() {
	std::cout << "JSON creation tests\n";
	using JSON::Value;
	JSON::Object o;

	o["string"] = Value::make("is a string");
	assert(o["string"]->type() == Value::Type::STRING);
	assert(o["string"]->str() == "is a string");

	o["bool"] = Value::make(true);
	assert(o["bool"]->type() == Value::Type::BOOLEAN);
	assert(o["bool"]->boolean());

	o["number"] = Value::make(42);
	assert(o["number"]->type() == Value::Type::NUMBER);
	assert(o["number"]->number() == 42);

	o["array"] = Value::make(JSON::Array {});
	assert(o["array"]->type() == Value::Type::ARRAY);

	o["object"] = Value::make(JSON::Object {});
	assert(o["object"]->type() == Value::Type::OBJECT);

	std::cout << "All tests passed\n------\n";
}

void test_parsing() {
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
	std::cout << "All tests passed\n------\n";
}

int main() {
	test_creation();
	test_parsing();
	return EXIT_SUCCESS;
}
