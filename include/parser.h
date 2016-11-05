#include <istream>
#include <cctype>
#include "json.h"

#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H 1
//----------------------------------------------------------------------------
namespace JSON {
namespace Parser {
//----------------------------------------------------------------------------
enum TokenKind {
	OB='{', OE='}', AB='[', AE=']',
	STRING='S', NUMBER='N', BOOL='B', EOS='\0',
        QUOT='"', ASSIGN=':', DELIM=',', UNEXPECTED='?'
};
//----------------------------------------------------------------------------
class Token {
	TokenKind k;
	std::string s;
	double d;
public:
	Token(TokenKind nk =EOS) :k{nk} {}
	Token(TokenKind nk, double nd) :k{nk}, d{nd} {}
	Token(TokenKind nk, std::string ns) :k{nk}, s{ns} {}

	inline std::string str() const { return s; }
	inline double number() const { return d; }

	inline TokenKind kind() const { return k; }
};
//----------------------------------------------------------------------------
class TokenStream  {
	std::istream& is;
	Token curr_tok;
	char* line;

public:
	TokenStream(std::istream& s) :is{s}, line{nullptr} {}
	~TokenStream() { if(line) delete[] line; };

	inline Token current() const { return curr_tok; }

	char const* tail();
	void raw_read(char*, int);
	Token get();
};
//----------------------------------------------------------------------------
JSON::Array _parse_array(TokenStream&);
JSON::Object _parse_object(TokenStream&);

JSON::Object parse(std::istream&);
JSON::Object parse(const std::string&);
//----------------------------------------------------------------------------
}} //namespace

#endif
