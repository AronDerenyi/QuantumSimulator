#include "Tokenizer.h"

Tokenizer::Exception::Exception(const Coordinate &coordinate) noexcept :
		runtime_error("Couldn't tokenize in \"" + coordinate.getFile() +
		              "\" at line " + std::to_string(coordinate.getLine()) +
		              " column " + std::to_string(coordinate.getColumn())) {

}

Tokenizer::Matcher::Matcher(std::string regex, Token::Type type) : regex(std::regex(regex)), type(type) {}

bool Tokenizer::Matcher::match(const Coordinate &coorinate, const std::string &code, Token &token) {
	static std::smatch match;
	if (std::regex_search(code, match, regex)) {
		token = Token(coorinate, type, match[1]);
		return true;
	} else {
		return false;
	}
}

Tokenizer::Matcher Tokenizer::matchers[] = {
		Matcher(R"(^(\s+))", Token::Type::NONE),
		Matcher(R"(^(//(.|[^\n])*\n))", Token::Type::NONE),

		Matcher(R"(^(OPENQASM)\W)", Token::Type::OPENQASM),
		Matcher(R"(^(include)\W)", Token::Type::INCLUDE),

		Matcher(R"(^(creg)\W)", Token::Type::CREG),
		Matcher(R"(^(qreg)\W)", Token::Type::QREG),
		Matcher(R"(^(gate)\W)", Token::Type::GATE),
		Matcher(R"(^(opaque)\W)", Token::Type::OPAQUE),
		Matcher(R"(^(if)\W)", Token::Type::IF),
		Matcher(R"(^(reset)\W)", Token::Type::RESET),
		Matcher(R"(^(measure)\W)", Token::Type::MEASURE),
		Matcher(R"(^(barrier)\W)", Token::Type::BARRIER),

		Matcher(R"(^(\())", Token::Type::LPARENTHESIS),
		Matcher(R"(^(\)))", Token::Type::RPARENTHESIS),
		Matcher(R"(^(\[))", Token::Type::LBRACKET),
		Matcher(R"(^(\]))", Token::Type::RBRACKET),
		Matcher(R"(^(\{))", Token::Type::GATE_BEGIN),
		Matcher(R"(^(\}))", Token::Type::GATE_END),

		Matcher(R"(^(\;))", Token::Type::SEMICOLON),
		Matcher(R"(^(\,))", Token::Type::COMMA),
		Matcher(R"(^(\+))", Token::Type::PLUS),
		Matcher(R"(^(\-)[^>])", Token::Type::MINUS),
		Matcher(R"(^(\*))", Token::Type::MUL),
		Matcher(R"(^(\/))", Token::Type::DIV),
		Matcher(R"(^(\^))", Token::Type::POW),
		Matcher(R"(^(\=\=))", Token::Type::EQUALS),
		Matcher(R"(^(\->))", Token::Type::ARROW),

		Matcher(R"(^([a-zA-Z_]\w*))", Token::Type::NAME),
		Matcher(R"(^(\d+)[^\w\.])", Token::Type::INTEGER),
		Matcher(R"(^(\d*\.\d+)[^\w])", Token::Type::REAL),
		Matcher(R"(^(\"((\\\")|[^\"])*\"))", Token::Type::STRING)
};

std::vector<Token> Tokenizer::tokenize(const std::string &file) {
	std::vector<Token> tokens;
	std::ifstream input(file);
	if (!input.good()) throw std::runtime_error("The file: \"" + file + "\" doesn't exist.");
	std::string code = (std::stringstream() << input.rdbuf()).str();

	Coordinate coordinate(file, 1, 1);
	Token token;

	while (!code.empty()) {
		bool matched = false;
		for (Matcher matcher : matchers) {
			if (matcher.match(coordinate, code, token)) {
				matched = true;
				code = code.substr(token.getValue().size());
				for (char c : token.getValue()) {
					if (c == '\n') {
						coordinate = Coordinate(coordinate.getFile(), coordinate.getLine() + 1, 1);
					} else if (c == '\r') {
						coordinate = Coordinate(coordinate.getFile(), coordinate.getLine(), 1);
					} else {
						coordinate = Coordinate(coordinate.getFile(), coordinate.getLine(), coordinate.getColumn() + 1);
					}
				}
				break;
			}
		}
		if (matched) {
			if (token.getType() != Token::Type::NONE) {
				tokens.push_back(token);
			}
		} else {
			throw Exception(coordinate);
		}
	}

	tokens.emplace_back(coordinate, Token::Type::END, "");
	return tokens;
}
