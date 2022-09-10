#include "token.h"

string Token::toString() {
	switch (this->type) {
	case TokenType::identifier:  return "IDENTIFIER: " + this->contents;
	case TokenType::int_literal: return "INT_LITERAL: " + this->contents;
	case TokenType::open_paren:  return "OPEN_PAREN";
	case TokenType::close_paren: return "CLOSE_PAREN";
	case TokenType::open_curly:  return "OPEN_CURLY";
	case TokenType::close_curly: return "CLOSE_CURLY";
	case TokenType::semicolon:   return "SEMICOLON";
	default:
		return "Token::toString() not implemented for token: "
			+ to_string(static_cast<int>(type));
	}
}
