#include "parser.h"

using namespace JSON;
//----------------------------------------------------------------------------
Parser::Token Parser::TokenStream::get() {
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
			curr_tok = {static_cast<TokenKind>(c)};
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
//----------------------------------------------------------------------------
char const* Parser::TokenStream::tail() {
	is.unget();
	if (!line) line = new char[22];
	raw_read(line+1, 20);
	line[0] = '>';
	line[21] = '<';

	is.get();
	return line;
}
//----------------------------------------------------------------------------
void Parser::TokenStream::raw_read(char* p, int n) {
	is.read(p, n);
	int i = n;
	while (i-- != 0)
		is.putback(p[i]);
}
//----------------------------------------------------------------------------
Array Parser::_parse_array(TokenStream& ts) {
	Array res;
	while (ts.get().kind() != EOS) {
		auto tok = ts.current();
		if (tok.kind() == STRING)
			res.push_back(Value::make(tok.str()));
		else if (tok.kind() == NUMBER)
			res.push_back(Value::make(
				tok.number()));
		else if (tok.kind() == BOOL)
			res.push_back(Value::make(
				bool(tok.number())));
		else if (tok.kind() == AB) {
			auto a {_parse_array(ts)};
			res.push_back(Value::make(std::move(a)));
			a.clear();
		}
		else if (tok.kind() == OB) {
			auto o {_parse_object(ts)};
			res.push_back(Value::make(std::move(o)));
			o.clear();
		}
		else if (tok.kind() == AE) return res;
		else throw Unexpected_token {ts.tail()};

		if (ts.get().kind() == AE) return res;
		if (ts.current().kind() == DELIM) continue;
		throw Unexpected_token("Expected ']' or ','");
	}
	throw Unexpected_eos {};
}
//----------------------------------------------------------------------------
Object Parser::_parse_object(TokenStream& ts) {
	Object res;
	while (ts.get().kind() != EOS) {
		if (ts.current().kind() == STRING) {
			auto key = ts.current().str();

			if (ts.get().kind() != ASSIGN)
				throw Unexpected_token {"Expected ':'"};

			auto tok = ts.get();
			if (tok.kind() == STRING)
				res[key] = Value::make(
					tok.str());
			else if (tok.kind() == NUMBER)
				res[key] = Value::make(
					tok.number());
			else if (tok.kind() == BOOL)
				res[key] = Value::make(
					bool(tok.number()));
			else if (tok.kind() == OB) {
				res[key] = Value::make(_parse_object(ts));
			}
			else if (tok.kind() == AB) {
				res[key] = Value::make(_parse_array(ts));
			}
			else throw Unexpected_token {ts.tail()};

			if (ts.get().kind() == OE) return res;
			if (ts.current().kind() == DELIM) continue;
		}
		else if (ts.current().kind() == OE)
			return res;
		throw Unexpected_token {"Expected '}' or ','"};
	}
	throw Unexpected_eos {};
}
//----------------------------------------------------------------------------
Object Parser::parse(std::istream& is) {
	TokenStream ts {is};
	if (ts.get().kind() == EOS) throw Unexpected_eos {"Stream is empty"};
	if (ts.current().kind() != OB) throw Unexpected_token {"Expected '{'"};
	Object res = _parse_object(ts);
	return res;
}
//----------------------------------------------------------------------------
