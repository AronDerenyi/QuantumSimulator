#ifndef QUANTUMSIMULATOR_ASTBUILDER_H
#define QUANTUMSIMULATOR_ASTBUILDER_H


#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

#include <sstream>
#include "AST.h"
#include "../tokenizer/Tokenizer.h"
#include "../tokenizer/Token.h"

namespace ast {

	/**
	 * Builds an abstract syntax tree from a vector of tokens.
	 */
	class Builder {
	public:

		/**
		 * A runtime error, thrown when the next token is not the expected one.
		 */
		class Exception : public std::runtime_error {
		public:

			explicit Exception(const Coordinate &coordinate) noexcept;
			explicit Exception(const Builder &builder) noexcept;
			explicit Exception(const Builder *builder) noexcept;
		};

	private:

		int pos = 0;
		std::vector<Token> tokens;

		/**
		 * Creates a builder that can build an abstract syntax tree
		 * from the given tokens.
		 *
		 * @param tokens The tokens from which the tree will be built
		 */
		explicit Builder(std::vector<Token> &tokens);

		/**
		 * Returns the current token.
		 *
		 * @return The current token
		 */
		Token get() const;

		/**
		 * Returns the current token. If the given type doesn't match
		 * with the current token's type, it throws an exception. After
		 * returning, it sets the current token as the next token
		 * in the token list.
		 *
		 * @param type The expected token type
		 * @return The current token
		 */
		Token eat(Token::Type type);

		/**
		 * Builds a program AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ProgramAST *program();

		/**
		 * Builds an include AST from the next tokens and tokenizes
		 * the included file. After tokenizing, it build an abstract
		 * syntax tree from the tokens and stores it.
		 *
		 * @return The built AST
		 */
		IncludeAST *include();

		/**
		 * Builds one of the program command ASTs from the next tokens.
		 *
		 * @return The built AST
		 */
		AST *program_command();

		/**
		 * Builds one of the gate command ASTs from the next tokens.
		 *
		 * @return The built AST
		 */
		AST *gate_command();

		/**
		 * Builds one of the condition command ASTs from the next tokens.
		 *
		 * @return The built AST
		 */
		AST *condition_command();

		/**
		 * Builds a creg AST from the next tokens.
		 *
		 * @return The built AST
		 */
		CRegAST *creg();

		/**
		 * Builds a creg declaration AST from the next tokens.
		 *
		 * @return The built AST
		 */
		CRegDeclarationAST *creg_declaration();

		/**
		 * Builds a qreg AST from the next tokens.
		 *
		 * @return The built AST
		 */
		QRegAST *qreg();

		/**
		 * Builds a qreg declaration AST from the next tokens.
		 *
		 * @return The built AST
		 */
		QRegDeclarationAST *qreg_declaration();

		/**
		 * Builds as many qreg ASTs as it can from the next tokens.
		 *
		 * @return The built ASTs
		 */
		std::vector<QRegAST *> qregs();

		/**
		 * Builds as many arguments as it can from the next tokens.
		 *
		 * @return The built arguments
		 */
		std::vector<std::string> arguments();

		/**
		 * Builds as many parameters as it can from the next tokens.
		 *
		 * @return The built parameters
		 */
		std::vector<std::string> parameters();

		/**
		 * Builds a gate declaration AST from the next tokens.
		 *
		 * @return The built AST
		 */
		GateDeclarationAST *gate_declaration();

		/**
		 * Builds an opaque declaration AST from the next tokens.
		 *
		 * @return The built AST
		 */
		OpaqueDeclarationAST *opaque_declaration();

		/**
		 * Builds as many expression ASTs as it can from the next tokens.
		 *
		 * @return The built ASTs
		 */
		std::vector<ExpressionAST *> expressions();

		/**
		 * Builds an expression AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ExpressionAST *expression();

		/**
		 * Builds an expression AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ExpressionAST *product();

		/**
		 * Builds an expression AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ExpressionAST *pow();

		/**
		 * Builds an expression AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ExpressionAST *value();

		/**
		 * Builds a gate AST from the next tokens.
		 *
		 * @return The built AST
		 */
		GateAST *gate();

		/**
		 * Builds a barrier AST from the next tokens.
		 *
		 * @return The built AST
		 */
		BarrierAST *barrier();

		/**
		 * Builds a reset AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ResetAST *reset();

		/**
		 * Builds a measure AST from the next tokens.
		 *
		 * @return The built AST
		 */
		MeasureAST *measure();

		/**
		 * Builds a condition AST from the next tokens.
		 *
		 * @return The built AST
		 */
		ConditionAST *condition();

		void *null();

	public:

		/**
		 * Creates an abstract syntax tree from the given tokens and
		 * returns it's root node (the program AST). It temporarily creates
		 * a builder object but discard it when the method returns.
		 *
		 * @param tokens The token from which the tree is created
		 * @return The root node of the tree
		 */
		static ProgramAST *build(std::vector<Token> tokens);
	};
}

using namespace ast;


#endif //QUANTUMSIMULATOR_ASTBUILDER_H
