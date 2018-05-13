#ifndef QUANTUMSIMULATOR_TOKEN_H
#define QUANTUMSIMULATOR_TOKEN_H

#include <string>
#include "Coordinate.h"

namespace tokenizer {

	/**
	 * A part of a source code, that contains it's characters
	 * in the code (value), it's position in the code (coordinate)
	 * and it's type (type, eg.: 12.34 -> Type::INTEGER).
	 */
	class Token {
	public:

		enum Type {
			NONE, END,
			OPENQASM, INCLUDE,
			CREG, QREG, GATE, OPAQUE, IF, RESET, MEASURE, BARRIER,
			LPARENTHESIS, RPARENTHESIS, LBRACKET, RBRACKET, GATE_BEGIN, GATE_END,
			SEMICOLON, COMMA, PLUS, MINUS, MUL, DIV, POW, EQUALS, ARROW,
			NAME, INTEGER, REAL, STRING
		};

	private:

		Coordinate coordinate;
		Type type;
		std::string value;

	public:

		/**
		 * Creates a token from the given constructor parameters.
		 *
		 * @param coordinate The coordinate of the token
		 * @param type The type of the token
		 * @param value The string value of the token
		 */
		Token(const Coordinate &coordinate = Coordinate(), const Type type = NONE, const std::string value = "");

		/**
		 * Copies the input token's values and creates a new token.
		 *
		 * @param token The input token
		 */
		Token(const Token &token);

		/**
		 * Copies the input token's values and sets them as the current token's values.
		 *
		 * @param token The input token
		 * @return The token itself
		 */
		Token& operator=(const Token &token);

		/**
		 * Returns the coordinates of the token.
		 *
		 * @return The coordinates of the token
		 */
		const Coordinate &getCoordinate() const;

		/**
		 * Returns the type of the token.
		 *
		 * @return The type of the token
		 */
		Type getType() const;

		/**
		 * Returns the corresponding string value of the token.
		 *
		 * @return The value of the token
		 */
		const std::string &getValue() const;

		/**
		 * Converts the string value of the token to an integer type.
		 *
		 * @return The value as an unsigned long
		 */
		unsigned long getInt();

		/**
		 * Converts the string value of the token to an real type.
		 *
		 * @return The value as a double
		 */
		double getReal();

		/**
		 * Converts the string value of the token to another string value,
		 * by removing the quotes from the beggining and the end, and
		 * converting the escaped (eg.: \", \\) characters to their
		 * corresponding character.
		 *
		 * @return The value as a string
		 */
		std::string getString();
	};
}

using namespace tokenizer;


#endif //QUANTUMSIMULATOR_TOKEN_H
