#include <sstream>
#include "json.h"

//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(bool b) {
	return new Bool_value {b};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(int i) {
	return new Integer_value {i};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(long l) {
	return new Integer_value {l};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(double d) {
	return new Real_value {d};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(const std::string& s) {
	return new String_value {s};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(Array&& a) {
	return new Array_value {std::forward<Array>(a)};
}
//----------------------------------------------------------------------------
JSON::Value* JSON::Value::make(Object&& o) {
	return new Object_value {std::forward<Object>(o)};
}
//----------------------------------------------------------------------------
std::ostream& JSON::operator<<(std::ostream& os, Array& a) {
	os << '[';
	bool no_comma {true};
	for (auto i : a) {
		if (no_comma) no_comma = false;
		else os << ", ";
		os << i;
	}
	os << ']';
	return os;
}
//----------------------------------------------------------------------------
std::ostream& JSON::operator<<(std::ostream& os, Object& o) {
	os << '{';
	bool no_comma {true};
	for (auto i : o) {
		if (no_comma) no_comma = false;
		else os << ", ";
		os << '"' << i.first << "\": " << i.second;
	}
	os << '}';
	return os;
}
//----------------------------------------------------------------------------
std::ostream& JSON::operator<<(std::ostream& os, Value& v) {
	if (v.type() == Value::value_type::REAL)
		os << v.real();
	if (v.type() == Value::value_type::INTEGER)
		os << v.integer();
	if (v.type() == Value::value_type::BOOLEAN)
		//os << (v.boolean() ? "true" : "false");
		os << std::boolalpha << v.boolean();
	else if (v.type() == Value::value_type::STRING)
		os << '"' << v.str() << '"';
	else if (v.type() == Value::value_type::OBJECT)
		os << v.object();
	else if (v.type() == Value::value_type::ARRAY)
		os << v.array();
	return os;
}
//----------------------------------------------------------------------------
std::ostream& JSON::operator<<(std::ostream& os, Value* v) {
	os << *v;
}
//----------------------------------------------------------------------------
JSON::Object JSON::parse(std::istream& is) {
	return JSON::Parser::parse(is);
}
//----------------------------------------------------------------------------
JSON::Object JSON::parse(std::string&& s) {
	std::istringstream is {s};
	return JSON::Parser::parse(is);
}
//----------------------------------------------------------------------------
JSON::Object JSON::parse(const std::string& s) {
	std::istringstream is {s};
	return JSON::Parser::parse(is);
}
//----------------------------------------------------------------------------
JSON::Object JSON::Literals::operator"" _object(const char* p, size_t sz) {
	return JSON::parse(p);
}
//----------------------------------------------------------------------------
JSON::Array JSON::Literals::operator"" _array(const char* p, size_t sz) {
	std::string s {"{\"result\":"};
	s += p; s.push_back('}');
	return std::move(JSON::parse(s)["result"]->array());
}
//----------------------------------------------------------------------------
