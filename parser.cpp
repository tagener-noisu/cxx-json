#include "parser.h"

//----------------------------------------------------------------------------
JSON::Array JSON::Parser::_parse_array(Token_stream& ts) {
	JSON::Array res;
	while (ts.get().kind() != EOS) {
		auto tok = ts.current();
		if (tok.kind() == STRING)
			res.push_back(JSON::Value::make(tok.str()));
		else if (tok.kind() == NUMBER)
			res.push_back(JSON::Value::make(
				tok.number()));
		else if (tok.kind() == BOOL)
			res.push_back(JSON::Value::make(
				bool(tok.number())));
		else if (tok.kind() == AB) {
			auto a {_parse_array(ts)};
			res.push_back(JSON::Value::make(std::move(a)));
			a.clear();
		}
		else if (tok.kind() == OB) {
			auto o {_parse_object(ts)};
			res.push_back(JSON::Value::make(std::move(o)));
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
JSON::Object JSON::Parser::_parse_object(Token_stream& ts) {
	JSON::Object res;
	while (ts.get().kind() != EOS) {
		if (ts.current().kind() == STRING) {
			auto key = ts.current().str();

			if (ts.get().kind() != ASSIGN)
				throw Unexpected_token {"Expected ':'"};

			auto tok = ts.get();
			if (tok.kind() == STRING)
				res[key] = JSON::Value::make(
					tok.str());
			else if (tok.kind() == NUMBER)
				res[key] = JSON::Value::make(
					tok.number());
			else if (tok.kind() == BOOL)
				res[key] = JSON::Value::make(
					bool(tok.number()));
			else if (tok.kind() == OB) {
				res[key] = JSON::Value::make(_parse_object(ts));
			}
			else if (tok.kind() == AB) {
				res[key] = JSON::Value::make(_parse_array(ts));
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
JSON::Object JSON::Parser::parse(std::istream& is) {
	Token_stream ts {is};
	if (ts.get().kind() == EOS) throw Unexpected_eos {"Stream is empty"};
	if (ts.current().kind() != OB) throw Unexpected_token {"Expected '{'"};
	JSON::Object res = _parse_object(ts);
	return res;
}
//----------------------------------------------------------------------------
