#ifndef QUANTUMSIMULATOR_SOURCECOORDINATE_H
#define QUANTUMSIMULATOR_SOURCECOORDINATE_H


#include <string>

namespace tokenizer {

	/**
	 * Represents a character in a source code by the source code's file,
	 * the character's line and the characters position in the line.
	 */
	class Coordinate {
	private:

		std::string file;
		int line;
		int column;

	public:

		/**
		 * Creates a coordinate from the given constructor parameters.
		 *
		 * @param file The file where the represented character is
		 * @param line The line where the character is
		 * @param column Position of the character in the line
		 */
		Coordinate(const std::string &file = "", int line = 0, int column = 0);

		/**
		 * Copies the input coordinate's values and creates a new coordinate.
		 *
		 * @param coordinate The input coordinate
		 */
		Coordinate(const Coordinate &coordinate);

		/**
		 * Copies the input coordinate's values and sets them as the current coordinate's values.
		 *
		 * @param coordinate The input coordinate
		 * @return The coordinate itself
		 */
		Coordinate &operator=(const Coordinate &coordinate);

		/**
		 * Returns the file where the source code is.
		 * @return The source code's file
		 */
		const std::string &getFile() const;

		/**
		 * Returns the line where the represented character is.
		 * @return The character's line's number
		 */
		int getLine() const;

		/**
		 * Returns the represented character's position in it's line.
		 * @return The character's position in it's line
		 */
		int getColumn() const;
	};
}

using namespace tokenizer;


#endif //QUANTUMSIMULATOR_SOURCECOORDINATE_H
