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

	char const* tail() {
		is.unget();
		if (!line) line = new char[22];
		raw_read(line+1, 20);
		line[0] = '>';
		line[21] = '<';
		is.get();
		return line;
	}

	void raw_read(char* p, int n) {
		is.read(p, n);
		int i = n;
		while (i-- != 0)
			is.putback(p[i]);
	}

	Token get() {
		char c;
		c = is.get();
		if (is.eof()) return Token {EOS};
		switch (c) {
			case OB:
			case OE:
			case AB:
			case AE:
			case ASSIGN:
			case DELIM:
				curr_tok = {static_cast<Token_kind>(c)};
				return curr_tok;
			case ' ':
			case '\n':
			case '\t':
				while (!is.eof()) {
					if (c != ' ' || c != '\n' || c != '\t')
						return get();
					c = is.get();
				}
				throw Unexpected_eos {};

			case 't':
			case 'f':
			{
				std::string s;
				while (!is.eof()) {
					if (isalpha(c)) s.push_back(c);
					else {
						throw Unexpected_token {tail()};
					}

					if (s.size() == 4 && s == "true") {
						curr_tok = {BOOL, true};
						return curr_tok;
					}
					else if (s.size() == 5 &&
						 s == "false")
					{
						curr_tok = {BOOL, false};
						return curr_tok;
					}
					else if (s.length() > 5) {
						while(s.size() != 0) {
							is.unget();
							s.pop_back();
						}
						throw Unexpected_token {tail()};
					}
					c = is.get();
				}
				throw Unexpected_eos {};
			}
			case QUOT:
			{
				std::string s;
				while (!is.eof()) { // return on QUOT
					c = is.get();
					if (c == '"' && s.back() != '\\') {
						curr_tok = {STRING, s};
						return curr_tok;
					}
					s.push_back(c);
				}
				throw Unterminated_string {};
			}
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': case '0':
			{
				is.putback(c);
				double d;
				is >> d;
				curr_tok = {NUMBER, d};
				return curr_tok;
			}
			case '\377':
				curr_tok = {EOS};
				return curr_tok;
			default:
				is.putback(c);
				curr_tok = {UNEXPECTED};
			return curr_tok;
		}
	}
};
//----------------------------------------------------------------------------
JSON::Array _parse_array(Token_stream&);
JSON::Object _parse_object(Token_stream&);

JSON::Object parse(std::istream&);
JSON::Object parse(const std::string&);
//----------------------------------------------------------------------------
}} //namespace

#endif
