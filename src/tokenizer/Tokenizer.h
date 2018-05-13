#ifndef QUANTUMSIMULATOR_TOKENIZER_H
#define QUANTUMSIMULATOR_TOKENIZER_H


#include <string>
#include <regex>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Token.h"

namespace tokenizer {

	/**
	 * Turns a given file to a vector of tokens.
	 */
	class Tokenizer {
	public:

		/**
		 * A runtime error, thrown when no matcher suitable for the source code found.
		 */
		class Exception : public std::runtime_error {
		public:

			Exception(const Coordinate& coordinate) noexcept;
		};

	private:

		/**
		 * Holds a token's type and the regex describing the token.
		 */
		class Matcher {
		private:

			std::regex regex;
			Token::Type type;

		public:

			/**
			 * Creates a matcher from the constructor parameters.
			 *
			 * @param regex The description of the token
			 * @param type The type of the token
			 */
			Matcher(std::string regex, Token::Type type);

			/**
			 * Tries to match the regex to the given code. If successful,
			 * true is returned and assigns the token reference to the matched token.
			 * If not, false is returned.
			 *
			 * @param coordinate The point in the source code where the matching happens
			 * @param code The given source code (not necessarily the entire code)
			 * @param token A reference to which the matched token will be assigned
			 * @return True if matched, false otherwise
			 */
			bool match(const Coordinate& coordinate, const std::string &code, Token &token);
		};

		static Matcher matchers[];

	public:

		/**
		 * Turns the given file into a vector of tokens,
		 * by scanning it and trying to match it to all of the matchers.
		 * If no matcher matches, a Tokenizer::Exception is thrown.
		 *
		 * @param file The given file's path
		 * @return The vector of tokens
		 */
		static std::vector<Token> tokenize(const std::string &file);
	};
}

using namespace tokenizer;


#endif //QUANTUMSIMULATOR_TOKENIZER_H
