#include <iostream>
#include "tokenizer/Tokenizer.h"
#include "ast/Builder.h"
#include "compiler/Program.h"
#include "compiler/Compiler.h"

void printUsage(std::string program) {
	std::cerr << "Usage: " << program << " <filename> <iterations>" << std::endl;
}

int main(int argc, const char *argv[]) {
	std::string programArgument = argv[0];
	std::string fileArgument;
	std::string iterationArgument;

#ifdef CPORTA

	// Getting arguments from the standard input
	std::cin >> fileArgument;
	std::cin >> iterationArgument;

#else

	// Checking argument count
	if (argc < 3 || argc > 3) {
		if (argc < 3) std::cerr << "Too few argument" << std::endl;
		if (argc > 3) std::cerr << "Too many arguments" << std::endl;
		printUsage(programArgument);
		return 1;
	}

	fileArgument = argv[1];
	iterationArgument = argv[2];

#endif

	// Checking argument validity
	for (int i = 0; i < iterationArgument.size(); i++) if (!isdigit(iterationArgument[i])) {
		std::cerr << "Invalid iteration count" << std::endl;
		printUsage(programArgument);
		return 1;
	}

	// Getting arguments
	std::string file = fileArgument;
	unsigned long iterations = std::stoul(iterationArgument);

	// Tokenizing and building the AST
	std::cout << "Tokenizing and building the Abstract Syntax Tree (this could take several seconds)..." << std::endl;
	std::vector<Token> tokens = Tokenizer::tokenize(file);
	ProgramAST *ast = Builder::build(tokens);

	// Compiling
	std::cout << "Compiling..." << std::endl;
	Program *p = Compiler::compile(ast);
	delete ast;

	// Executing
	std::cout << "Executing..." << std::endl;
	for (int i = 0; i < iterations; i++) {
		p->execute();
		double div = ((double) iterations) / 10;
		if (i != 0 && (int) (i / div) != (int) ((i - 1) / div)) {
			std::cout << (int) (i / div) << "0% ";
			std::cout.flush();
		}
	}
	std::cout << "100%" << std::endl;

	// Printing results
	std::cout << std::endl << "Results: " << std::endl;
	p->printResults();

	delete p;
	return 0;
}
