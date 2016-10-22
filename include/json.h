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
struct destroyable_map;

template<typename _Tp>
struct destroyable_vec;

using Object = destroyable_map<std::string, Value*>;
using Array = destroyable_vec<Value*>;
//----------------------------------------------------------------------------
template <typename _K, typename _V>
struct destroyable_map : public std::unordered_map<_K, _V> {
	using _Base = std::unordered_map<_K, _V>;

	destroyable_map() =default;
	destroyable_map(destroyable_map&&) =default;

	destroyable_map& operator=(destroyable_map&&) =default;

	void destroy() {
		for (auto i : *this)
			delete i.second;
		this->clear();
	}

	~destroyable_map() {
		if (this->size() > 0) destroy();
	}
private:
	destroyable_map(const destroyable_map&);
};
//----------------------------------------------------------------------------
template <typename _Tp>
struct destroyable_vec : public std::vector<_Tp> {
	using _Base = std::vector<_Tp>;

	destroyable_vec() =default;
	destroyable_vec(destroyable_vec&&) =default;

	destroyable_vec& operator=(destroyable_vec&&) =default;

	void destroy() {
		for (auto i : *this)
			delete i;
		this->clear();
	}

	~destroyable_vec() {
		if (this->size() > 0) destroy();
	}
private:
	destroyable_vec(const destroyable_vec&);
};
//----------------------------------------------------------------------------
struct Value {
	enum class Type {
		INTEGER, REAL, STRING, OBJECT, ARRAY, BOOLEAN
	};
	using value_type = Type;

	struct Wrong_type : public std::runtime_error {
		Wrong_type(const char* s) :runtime_error{s} {};
	};

	virtual value_type type() const =0;

	virtual bool& boolean() { throw Wrong_type {"Not a boolean"}; }
	virtual double& real() { throw Wrong_type {"Not a number"}; }
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
class Bool_value : public Value {
	bool b;
public:
	Bool_value(bool nb) :b{nb} {}

	value_type type() const override { return value_type::BOOLEAN; }
	bool& boolean() override { return b; }
};
//----------------------------------------------------------------------------
class Real_value : public Value {
	double d;
public:
	Real_value(double nd) :d{nd} {}

	value_type type() const override { return value_type::REAL; }
	double& real() override { return d; }
};
//----------------------------------------------------------------------------
class String_value : public Value {
	std::string s;
public:
	String_value(const std::string& ns) :s{ns} {}

	value_type type() const override { return value_type::STRING; }
	std::string& str() override { return s; }
};
//----------------------------------------------------------------------------
class Object_value : public Value {
	Object o;
public:
	Object_value(Object&& no) :o{std::move(no)} {}

	value_type type() const override { return value_type::OBJECT; }
	Object& object() override { return o; }
};
//----------------------------------------------------------------------------
class Array_value : public Value {
	Array a;
public:
	Array_value(Array&& na) :a{std::forward<Array>(na)} {}

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
