#include <iostream>
#include <vector>
#include <unordered_map>
//----------------------------------------------------------------------------
#ifndef _JSON_H
#define _JSON_H 1
//----------------------------------------------------------------------------
namespace JSON {
//----------------------------------------------------------------------------
struct Value;

template<typename _K, typename _V>
struct DestroyableMap;

template<typename _Tp>
struct DestroyableVector;

using Object = DestroyableMap<std::string, Value*>;
using Array = DestroyableVector<Value*>;
//----------------------------------------------------------------------------
template <typename _K, typename _V>
class DestroyableMap : public std::unordered_map<_K, _V> {
public:
	using _Base = std::unordered_map<_K, _V>;

	DestroyableMap() =default;
	DestroyableMap(DestroyableMap&&) =default;

	DestroyableMap& operator=(DestroyableMap&&) =default;

	void destroy() {
		for (auto i : *this)
			delete i.second;
		this->clear();
	}

	~DestroyableMap() {
		if (this->size() > 0) destroy();
	}
private:
	DestroyableMap(const DestroyableMap&);
};
//----------------------------------------------------------------------------
template <typename _Tp>
class DestroyableVector : public std::vector<_Tp> {
public:
	using _Base = std::vector<_Tp>;

	DestroyableVector() =default;
	DestroyableVector(DestroyableVector&&) =default;

	DestroyableVector& operator=(DestroyableVector&&) =default;

	void destroy() {
		for (auto i : *this)
			delete i;
		this->clear();
	}

	~DestroyableVector() {
		if (this->size() > 0) destroy();
	}
private:
	DestroyableVector(const DestroyableVector&);
};
//----------------------------------------------------------------------------
struct Value {
	enum class Type {
		NUMBER, STRING, OBJECT, ARRAY, BOOLEAN
	};
	using value_type = Type;

	struct Wrong_type : public std::runtime_error {
		Wrong_type(const char* s) :runtime_error{s} {};
	};

	virtual value_type type() const =0;

	virtual bool& boolean() { throw Wrong_type {"Not a boolean"}; }
	virtual double& number() { throw Wrong_type {"Not a number"}; }
	virtual std::string& str() { throw Wrong_type {"Not a string"}; }
	virtual Array& array() { throw Wrong_type {"Not an array"}; }
	virtual Object& object() { throw Wrong_type {"Not an object"}; }

	virtual ~Value() { }

	static Value* make(bool);
	static Value* make(int);
	static Value* make(double);
	static Value* make(std::string);
	static Value* make(Array&&);
	static Value* make(Object&&);
};
//----------------------------------------------------------------------------
class BoolValue : public Value {
	bool b;
public:
	BoolValue(bool nb) :b{nb} {}

	value_type type() const override { return value_type::BOOLEAN; }
	bool& boolean() override { return b; }
};
//----------------------------------------------------------------------------
class NumberValue : public Value {
	double d;
public:
	NumberValue(double nd) :d{nd} {}

	value_type type() const override { return value_type::NUMBER; }
	double& number() override { return d; }
};
//----------------------------------------------------------------------------
class StringValue : public Value {
	std::string s;
public:
	StringValue(const std::string& ns) :s{ns} {}

	value_type type() const override { return value_type::STRING; }
	std::string& str() override { return s; }
};
//----------------------------------------------------------------------------
class ObjectValue : public Value {
	Object o;
public:
	ObjectValue(Object&& no) :o{std::move(no)} {}

	value_type type() const override { return value_type::OBJECT; }
	Object& object() override { return o; }
};
//----------------------------------------------------------------------------
class ArrayValue : public Value {
	Array a;
public:
	ArrayValue(Array&& na) :a{std::forward<Array>(na)} {}

	value_type type() const override { return value_type::ARRAY; }
	Array& array() override { return a; }
};
//----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream&, Value*);
std::ostream& operator<<(std::ostream&, Value&);
std::ostream& operator<<(std::ostream&, Object&);
std::ostream& operator<<(std::ostream&, Array&);
//----------------------------------------------------------------------------
namespace Parser {
Object parse(std::istream&);
//----------------------------------------------------------------------------
struct Error : public std::runtime_error {
	Error() :runtime_error{""} {}
	Error(const char* s) :runtime_error{s} {}
};

struct Unterminated_string : public Error {};

struct Unexpected_eos : public Error {
	Unexpected_eos() =default;
	Unexpected_eos(const char* s) :Error{s} {}
};

struct Unexpected_token : public Error {
	Unexpected_token() =default;
	Unexpected_token(const char* s) :Error{s} {}
};
}
//----------------------------------------------------------------------------
Object parse(std::istream&);
Object parse(std::string&&);
Object parse(const std::string&);
//----------------------------------------------------------------------------
namespace Literals {
	JSON::Object operator"" _object(const char*, size_t);
	JSON::Array operator"" _array(const char*, size_t);
}
//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------
#endif
