#include "Token.h"

Token::Token(const Coordinate &coordinate, const Token::Type type, const std::string value) : coordinate(coordinate), type(type), value(value) {}

Token::Token(const Token &token) {
	*this = token;
}

Token& Token::operator=(const Token &token) {
	coordinate = token.coordinate;
	type = token.type;
	value = token.value;
	return *this;
}

const Coordinate &Token::getCoordinate() const {
	return coordinate;
}

Token::Type Token::getType() const {
	return type;
}

const std::string &Token::getValue() const {
	return value;
}

unsigned long Token::getInt() {
	return std::stoul(value);
}

double Token::getReal() {
	return std::stod(value);
}

std::string Token::getString() {
	if (value[0] != '\"') return value;

	std::string string;
	for (unsigned long i = 1; i < value.size() - 1; i++) {
		if (value[i] == '\\') {
			i++;
			if (value[i] == '\\') {
				string += '\\';
			} else if (value[i] == '\'') {
				string += '\'';
			} else if (value[i] == '\"') {
				string += '\"';
			} else if (value[i] == 'a') {
				string += '\a';
			} else if (value[i] == 'b') {
				string += '\b';
			} else if (value[i] == 'f') {
				string += '\f';
			} else if (value[i] == 'n') {
				string += '\n';
			} else if (value[i] == 'r') {
				string += '\r';
			} else if (value[i] == 't') {
				string += '\t';
			} else if (isdigit(value[i])) {
				int n = 0;
				while (isdigit(value[i])) {
					n = n * 10 + value[i] - 48;
					i++;
				}
				string += (char) n;
				i--;
			}
		} else {
			string += value[i];
		}
	}
	return string;
}
