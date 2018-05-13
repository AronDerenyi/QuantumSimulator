#include "Builder.h"

Builder::Exception::Exception(const Coordinate &coordinate) noexcept :
		runtime_error("Unexpected token in \"" + coordinate.getFile() +
		              "\" at line " + std::to_string(coordinate.getLine()) +
		              " column " + std::to_string(coordinate.getColumn())) {

}

Builder::Exception::Exception(const Builder &builder) noexcept :
		Exception(builder.get().getCoordinate()) {

}

Builder::Exception::Exception(const Builder *builder) noexcept :
		Exception(*builder) {

}



ProgramAST *Builder::build(std::vector<Token> tokens) {
	return Builder(tokens).program();
}

Builder::Builder(std::vector<Token> &tokens) : tokens(tokens) {

}

Token Builder::get() const {
	return tokens[pos];
}

Token Builder::eat(Token::Type type) {
	if (get().getType() != type) throw Exception(this);
	Token token = get();
	pos++;
	return token;
}



ProgramAST *Builder::program() {
	// program: OPENQASM REAL SEMICOLON program_command* END

	Coordinate coordinate;
	double version;
	std::vector<AST *> commands;
	AST *command;

	coordinate = eat(Token::OPENQASM).getCoordinate();
	version = eat(Token::REAL).getReal();
	eat(Token::SEMICOLON);
	while ((command = program_command()) != nullptr) {
		commands.push_back(command);
	}
	eat(Token::END);

	return new ProgramAST(coordinate, version, commands);
}

IncludeAST *Builder::include() {
	// include: INCLUDE STRING SEMICOLON

	Coordinate coordinate;
	std::string file;
	std::vector<AST *> commands;
	AST *command;

	coordinate = eat(Token::INCLUDE).getCoordinate();
	file = eat(Token::STRING).getString();
	eat(Token::SEMICOLON);

	std::vector<Token> tokens;
	if (file[0] == PATH_SEPARATOR) {
		tokens = Tokenizer::tokenize(file);
	} else {
		const std::string &currentFile = coordinate.getFile();
		unsigned long lastPathSeparator = currentFile.find_last_of(PATH_SEPARATOR);
		tokens = Tokenizer::tokenize(currentFile.substr(0, lastPathSeparator + 1) + file);
	}

	Builder builder(tokens);
	while ((command = builder.program_command()) != nullptr) {
		commands.push_back(command);
	}
	builder.eat(Token::END);

	return new IncludeAST(coordinate, commands);
}



AST *Builder::program_command() {
	// program_command: include | creg_declaration | qreg_declaration | gate_declaration | opaque_declaration | gate | barrier | reset | measure | condition

	switch (get().getType()) {
		case Token::INCLUDE:
			return include();
		case Token::CREG:
			return creg_declaration();
		case Token::QREG:
			return qreg_declaration();
		case Token::GATE:
			return gate_declaration();
		case Token::OPAQUE:
			return opaque_declaration();
		case Token::NAME:
			return gate();
		case Token::BARRIER:
			return barrier();
		case Token::RESET:
			return reset();
		case Token::MEASURE:
			return measure();
		case Token::IF:
			return condition();
		default:
			return (AST *) null();
	}
}

AST *Builder::gate_command() {
	// gate_command: gate | barrier

	switch (get().getType()) {
		case Token::NAME:
			return gate();
		case Token::BARRIER:
			return barrier();
		default:
			return (AST *) null();
	}
}

AST *Builder::condition_command() {
	// condition_command: gate | reset | measure

	switch (get().getType()) {
		case Token::NAME:
			return gate();
		case Token::RESET:
			return reset();
		case Token::MEASURE:
			return measure();
		default:
			throw Exception(this);
	}
}



CRegAST *Builder::creg() {
	// creg: NAME (LBRACKET INTEGER RBRACKET)?

	Coordinate coordinate;
	std::string name;
	bool indexed;
	unsigned long index;

	Token tmp = eat(Token::NAME);
	coordinate = tmp.getCoordinate();
	name = tmp.getValue();
	if (get().getType() == Token::LBRACKET) {
		eat(Token::LBRACKET);
		indexed = true;
		index = eat(Token::INTEGER).getInt();
		eat(Token::RBRACKET);
	} else {
		indexed = false;
		index = 0;
	}

	return new CRegAST(coordinate, name, indexed, index);
}

CRegDeclarationAST *Builder::creg_declaration() {
	// creg_declaration: CREG NAME LBRACKET INTEGER RBRACKET SEMICOLON

	Coordinate coordinate;
	std::string name;
	unsigned long size;

	coordinate = eat(Token::CREG).getCoordinate();
	name = eat(Token::NAME).getValue();
	eat(Token::LBRACKET);
	size = eat(Token::INTEGER).getInt();
	eat(Token::RBRACKET);
	eat(Token::SEMICOLON);

	return new CRegDeclarationAST(coordinate, name, size);
}



QRegAST *Builder::qreg() {
	// qreg: NAME (LBRACKET INTEGER RBRACKET)?

	Coordinate coordinate;
	std::string name;
	bool indexed;
	unsigned long index;

	Token tmp = eat(Token::NAME);
	coordinate = tmp.getCoordinate();
	name = tmp.getValue();
	if (get().getType() == Token::LBRACKET) {
		eat(Token::LBRACKET);
		indexed = true;
		index = eat(Token::INTEGER).getInt();
		eat(Token::RBRACKET);
	} else {
		indexed = false;
		index = 0;
	}

	return new QRegAST(coordinate, name, indexed, index);
}

QRegDeclarationAST *Builder::qreg_declaration() {
	// qreg_declaration: QREG NAME LBRACKET INTEGER RBRACKET SEMICOLON

	Coordinate coordinate;
	std::string name;
	unsigned long size;

	coordinate = eat(Token::QREG).getCoordinate();
	name = eat(Token::NAME).getValue();
	eat(Token::LBRACKET);
	size = eat(Token::INTEGER).getInt();
	eat(Token::RBRACKET);
	eat(Token::SEMICOLON);

	return new QRegDeclarationAST(coordinate, name, size);
}



std::vector<QRegAST *> Builder::qregs() {
	// qregs: qreg (COMMA qreg)*

	std::vector<QRegAST *> qregs;

	qregs.push_back(qreg());
	while (get().getType() == Token::COMMA) {
		eat(Token::COMMA);
		qregs.push_back(qreg());
	}

	return qregs;
}

std::vector<std::string> Builder::arguments() {
	// arguments: NAME (COMMA NAME)*

	std::vector<std::string> arguments;

	arguments.push_back(eat(Token::NAME).getValue());
	while (get().getType() == Token::COMMA) {
		eat(Token::COMMA);
		arguments.push_back(eat(Token::NAME).getValue());
	}

	return arguments;
}

std::vector<std::string> Builder::parameters() {
	// parameters: (NAME (COMMA NAME)*)?

	std::vector<std::string> parameters;

	if (get().getType() == Token::NAME) {
		parameters.push_back(eat(Token::NAME).getValue());
		while (get().getType() == Token::COMMA) {
			eat(Token::COMMA);
			parameters.push_back(eat(Token::NAME).getValue());
		}
	}

	return parameters;
}



GateDeclarationAST *Builder::gate_declaration() {
	// gate_declaration: GATE NAME (LPARENTHESIS parameters RPARENTHESIS)? arguments GATE_BEGIN gate_command* GATE_END

	Coordinate coordinate;
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::string> arguments;
	std::vector<AST *> commands;
	AST *command;

	coordinate = eat(Token::GATE).getCoordinate();
	name = eat(Token::NAME).getValue();
	if (get().getType() == Token::LPARENTHESIS) {
		eat(Token::LPARENTHESIS);
		parameters = Builder::parameters();
		eat(Token::RPARENTHESIS);
	}
	arguments = Builder::arguments();
	eat(Token::GATE_BEGIN);
	while ((command = gate_command()) != nullptr) {
		commands.push_back(command);
	}
	eat(Token::GATE_END);

	return new GateDeclarationAST(coordinate, name, parameters, arguments, commands);
}

OpaqueDeclarationAST *Builder::opaque_declaration() {
	// opaque_declaration: OPAQUE NAME (LPARENTHESIS parameters RPARENTHESIS)? arguments SEMICOLON

	Coordinate coordinate;
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::string> arguments;

	coordinate = eat(Token::OPAQUE).getCoordinate();
	name = eat(Token::NAME).getValue();
	if (get().getType() == Token::LPARENTHESIS) {
		eat(Token::LPARENTHESIS);
		parameters = Builder::parameters();
		eat(Token::RPARENTHESIS);
	}
	arguments = Builder::arguments();
	eat(Token::SEMICOLON);

	return new OpaqueDeclarationAST(coordinate, name, parameters, arguments);
}



std::vector<ExpressionAST *> Builder::expressions() {
	// expressions: (expression (COMMA expression)*)?

	std::vector<ExpressionAST *> expressions;

	if (get().getType() == Token::LPARENTHESIS ||
	    get().getType() == Token::PLUS ||
	    get().getType() == Token::MINUS ||
	    get().getType() == Token::INTEGER ||
	    get().getType() == Token::REAL ||
	    get().getType() == Token::NAME) {

		expressions.push_back(expression());
		while (get().getType() == Token::COMMA) {
			eat(Token::COMMA);
			expressions.push_back(expression());
		}
	}

	return expressions;
}

ExpressionAST *Builder::expression() {
	// expression: product ((PLUS | MINUS) product)*

	char operation;
	ExpressionAST *expression;

	expression = product();
	while (get().getType() == Token::PLUS || get().getType() == Token::MINUS) {
		Token tmp = eat(get().getType());
		operation = tmp.getValue()[0];
		expression = new OperationAST(tmp.getCoordinate(), operation, expression, product());
	}

	return expression;
}

ExpressionAST *Builder::product() {
	// product: pow ((MUL | DIV) pow)*

	char operation;
	ExpressionAST *expression;

	expression = pow();
	while (get().getType() == Token::MUL || get().getType() == Token::DIV) {
		Token tmp = eat(get().getType());
		operation = tmp.getValue()[0];
		expression = new OperationAST(tmp.getCoordinate(), operation, expression, pow());
	}

	return expression;
}

ExpressionAST *Builder::pow() {
	//pow: value (POW value)*

	char operation;
	ExpressionAST *expression;

	expression = value();
	while (get().getType() == Token::POW) {
		Token tmp = eat(get().getType());
		operation = tmp.getValue()[0];
		return new OperationAST(tmp.getCoordinate(), operation, expression, value());
	}

	return expression;
}

ExpressionAST *Builder::value() {
	// value: (LPARENTHESIS expression RPARENTHESIS) | ((PLUS | MINUS) expression) | INTEGER | REAL | NAME (LPARENTHESIS expression RPARENTHESIS)?

	Coordinate coordinate;
	ExpressionAST *expression;

	char operation;
	unsigned long integer;
	double real;
	std::string name;

	switch (get().getType()) {
		case Token::LPARENTHESIS:
			eat(Token::LPARENTHESIS);
			expression = Builder::expression();
			eat(Token::RPARENTHESIS);
			return expression;
		case Token::PLUS:
		case Token::MINUS:
			operation = get().getValue()[0];
			coordinate = eat(get().getType()).getCoordinate();
			expression = Builder::expression();
			return new OperationAST(coordinate, operation, new ValueAST(coordinate, 0), expression);
		case Token::INTEGER:
			integer = get().getInt();
			coordinate = eat(Token::INTEGER).getCoordinate();
			return new ValueAST(coordinate, integer);
		case Token::REAL:
			real = get().getReal();
			coordinate = eat(Token::REAL).getCoordinate();
			return new ValueAST(coordinate, real);
		case Token::NAME:
			name = get().getValue();
			coordinate = eat(Token::NAME).getCoordinate();
			if (get().getType() == Token::LPARENTHESIS) {
				eat(Token::LPARENTHESIS);
				expression = Builder::expression();
				eat(Token::RPARENTHESIS);
				return new FunctionAST(coordinate, name, expression);
			} else {
				return new ConstantAST(coordinate, name);
			}
		default:
			throw Exception(this);
	}
}



GateAST *Builder::gate() {
	// gate: NAME (LPARENTHESIS expressions RPARENTHESIS)? qregs SEMICOLON

	Coordinate coordinate;
	std::string name;
	std::vector<ExpressionAST *> expressions;
	std::vector<QRegAST *> qregs;

	Token tmp = eat(Token::NAME);
	coordinate = tmp.getCoordinate();
	name = tmp.getValue();
	if (get().getType() == Token::LPARENTHESIS) {
		eat(Token::LPARENTHESIS);
		expressions = Builder::expressions();
		eat(Token::RPARENTHESIS);
	}
	qregs = Builder::qregs();
	eat(Token::SEMICOLON);

	return new GateAST(coordinate, name, expressions, qregs);
}

BarrierAST *Builder::barrier() {
	// barrier: BARRIER qreg SEMICOLON

	Coordinate coordinate;
	std::vector<QRegAST *> qregs;

	coordinate = eat(Token::BARRIER).getCoordinate();
	qregs = Builder::qregs();
	eat(Token::SEMICOLON);

	return new BarrierAST(coordinate, qregs);
}

ResetAST *Builder::reset() {
	// reset: RESET qreg SEMICOLON

	Coordinate coordinate;
	QRegAST *qreg;

	coordinate = eat(Token::RESET).getCoordinate();
	qreg = Builder::qreg();
	eat(Token::SEMICOLON);

	return new ResetAST(coordinate, qreg);
}

MeasureAST *Builder::measure() {
	// measure: MEASURE qreg ARROW creg SEMICOLON

	Coordinate coordinate;
	QRegAST *qreg;
	CRegAST *creg;

	coordinate = eat(Token::MEASURE).getCoordinate();
	qreg = Builder::qreg();
	eat(Token::ARROW);
	creg = Builder::creg();
	eat(Token::SEMICOLON);

	return new MeasureAST(coordinate, qreg, creg);
}

ConditionAST *Builder::condition() {
	//condition: IF LPARENTHESIS creg EQUALS INTEGER RPARENTHESIS condition_command

	Coordinate coordinate;
	CRegAST *creg;
	unsigned long integer;
	AST *command;

	coordinate = eat(Token::IF).getCoordinate();
	eat(Token::LPARENTHESIS);
	creg = Builder::creg();
	eat(Token::EQUALS);
	integer = eat(Token::INTEGER).getInt();
	eat(Token::RPARENTHESIS);
	command = condition_command();

	return new ConditionAST(coordinate, creg, integer, command);
}



void *Builder::null() {
	// null:
	return nullptr;
}
