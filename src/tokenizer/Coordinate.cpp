#include "Coordinate.h"

Coordinate::Coordinate(const std::string &file, int line, int column) : file(file), line(line), column(column) {

}

Coordinate::Coordinate(const Coordinate &coordinate) {
	*this = coordinate;
}

Coordinate &Coordinate::operator=(const Coordinate &coordinate) {
	file = coordinate.file;
	line = coordinate.line;
	column = coordinate.column;
	return *this;
}

const std::string &Coordinate::getFile() const {
	return file;
}

int Coordinate::getLine() const {
	return line;
}

int Coordinate::getColumn() const {
	return column;
}
