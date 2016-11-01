#include <istream>
#include <cctype>
#include "json.h"

#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H 1
//----------------------------------------------------------------------------
namespace JSON {
namespace Parser {
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
//----------------------------------------------------------------------------
enum Token_kind {
	OB='{', OE='}', AB='[', AE=']',
	STRING='S', NUMBER='N', BOOL='B', EOS='\0',
        QUOT='"', ASSIGN=':', DELIM=',', UNEXPECTED='?'
};
//----------------------------------------------------------------------------
class Token {
	Token_kind k;
	std::string s;
	double d;
public:
	Token(Token_kind nk =EOS) :k{nk} {}
	Token(Token_kind nk, double nd) :k{nk}, d{nd} {}
	Token(Token_kind nk, std::string ns) :k{nk}, s{ns} {}

	inline std::string str() const { return s; }
	inline double number() const { return d; }

	inline Token_kind kind() const { return k; }
};
//----------------------------------------------------------------------------
class Token_stream  {
	std::istream& is;
	Token curr_tok;
	char* line;

public:
	Token_stream(std::istream& s) :is{s}, line{nullptr} {}
	~Token_stream() { if(line) delete[] line; };

	inline Token current() const { return curr_tok; }

	char const* tail();
	void raw_read(char*, int);
	Token get();
};
//----------------------------------------------------------------------------
JSON::Array _parse_array(Token_stream&);
JSON::Object _parse_object(Token_stream&);

JSON::Object parse(std::istream&);
JSON::Object parse(const std::string&);
//----------------------------------------------------------------------------
}} //namespace

#endif
