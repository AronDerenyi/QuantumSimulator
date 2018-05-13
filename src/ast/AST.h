#ifndef QUANTUMSIMULATOR_AST_H
#define QUANTUMSIMULATOR_AST_H


#include <vector>
#include "Builder.h"
#include "../math/Environment.h"
#include "../tokenizer/Token.h"

namespace ast { namespace asts {

	/**
	 * Represents a node in an abstract syntax tree.
	 */
	class AST {
	public:

		/**
		 * The possible abstract syntax tree node types
		 */
		enum Type {
			PROGRAM, INCLUDE,
			CREG, CREG_DECLARATION,
			QREG, QREG_DECLARATION,
			GATE_DECLARATION, OPAQUE_DECLARATION,
			EXPRESSION,
			CONDITION, GATE, RESET, MEASURE, BARRIER
		};

	private:

		Coordinate coordinate;
		Type type;

	public:

		/**
		 * Creates an abstract syntax tree node with the given type.
		 * The coordinate is needed so that error messages can be more detailed.
		 *
		 * @param coordinate The node's coordinate in the source code
		 * @param type The node's type
		 */
		AST(const Coordinate &coordinate, Type type);

		/**
		 * Deletes the abstract syntax tree node's children.
		 * Every node is responsible for the deletion of its children.
		 */
		virtual ~AST();

		/**
		 * Returns the node's coordinate in the source code.
		 *
		 * @return The node's coordinate
		 */
		const Coordinate &getCoordinate() const;

		/**
		 * Returns the node's type
		 *
		 * @return The node's type
		 */
		Type getType() const;
	};



	/**
	 * Represents the whole program itself by it's version
	 * and all of it's commands.
	 */
	class ProgramAST : public AST {
	private:

		double version;
		std::vector<AST *> commands;

	public:

		ProgramAST(const Coordinate &coordinate, double version, const std::vector<AST *> &commands);
		~ProgramAST() override;

		double getVersion() const;
		const std::vector<AST *> &getCommands() const;
	};

	/**
	 * Represents an included source code by the included commands
	 */
	class IncludeAST : public AST {
	private:

		std::vector<AST *> commands;

	public:

		IncludeAST(const Coordinate &coordinate, std::vector<AST *> &commands);
		~IncludeAST() override;

		const std::vector<AST *> &getCommands() const;
	};



	/**
	 * Represents a real register by it's name (and possibly one of the bit's index).
	 */
	class CRegAST : public AST {
	private:

		std::string name;
		bool indexed;
		unsigned long index;

	public:

		CRegAST(const Coordinate &coordinate, const std::string &name, bool indexed, unsigned long index);

		const std::string &getName() const;
		bool isIndexed() const;
		unsigned long getIndex() const;
	};

	/**
	 * Represents a real register's declaration by the registers name
	 * and it's number of bits.
	 */
	class CRegDeclarationAST : public AST {
	private:

		std::string name;
		unsigned long size;

	public:

		CRegDeclarationAST(const Coordinate &coordinate, const std::string &name, unsigned long size);

		const std::string &getName() const;
		unsigned long getSize() const;
	};



	/**
	 * Represents a quantum register by it's name (and possibly one of the qubit's index).
	 */
	class QRegAST : public AST {
	private:

		std::string name;
		bool indexed;
		unsigned long index;

	public:

		QRegAST(const Coordinate &coordinate, const std::string &name, bool indexed, unsigned long index);

		const std::string &getName() const;
		bool isIndexed() const;
		unsigned long getIndex() const;
	};

	/**
	 * Represents a quantum register's declaration by the registers name
	 * and it's number of qubits.
	 */
	class QRegDeclarationAST : public AST {
	private:

		std::string name;
		unsigned long size;

	public:

		QRegDeclarationAST(const Coordinate &coordinate, const std::string &name, unsigned long size);

		const std::string &getName() const;
		unsigned long getSize() const;
	};



	/**
	 * Represents a gate's declaration by it's name, parameters, arguments
	 * and it's commands.
	 */
	class GateDeclarationAST : public AST {
	private:

		std::string name;
		std::vector<std::string> parameters;
		std::vector<std::string> arguments;
		std::vector<AST *> commands;

	public:

		GateDeclarationAST(const Coordinate &coordinate, const std::string &name, const std::vector<std::string> &parameters, const std::vector<std::string> &arguments, const std::vector<AST *> &commands);
		~GateDeclarationAST() override;

		const std::string &getName() const;
		const std::vector<std::string> &getParameters() const;
		const std::vector<std::string> &getArguments() const;
		const std::vector<AST *> &getCommands() const;
	};

	/**
	 * Represents an opaque declaration by it's name, parameters and arguments
	 */
	class OpaqueDeclarationAST : public AST {
	private:

		std::string name;
		std::vector<std::string> parameters;
		std::vector<std::string> arguments;

	public:

		OpaqueDeclarationAST(const Coordinate &coordinate, const std::string &name, const std::vector<std::string> &parameters, const std::vector<std::string> &arguments);

		const std::string &getName() const;
		const std::vector<std::string> &getParameters() const;
		const std::vector<std::string> &getArguments() const;
	};



	/**
	 * Represents a mathematical expression.
	 */
	class ExpressionAST : public AST {
	public:

		/**
		 * The possible expression types
		 */
		enum ExpressionType {
			OPERATION,
			VALUE,
			CONSTANT,
			FUNCTION
		};

	private:

		ExpressionType expressionType;

	public:

		/**
		 * Creates and expression from it's coordinates and it's type.
		 *
		 * @param coordinate The expression's coordinate
		 * @param expressionType The expression's type
		 */
		ExpressionAST(const Coordinate &coordinate, ExpressionType expressionType);

		/**
		 * Returns the expression's type.
		 *
		 * @return the expression's type
		 */
		ExpressionType getExpressionType() const;
	};

	/**
	 * Represent's a mathematical operation by it's operands as expressions
	 * and it's operation as a character.
	 * Possible operations: +, -, *, /, ^
	 */
	class OperationAST : public ExpressionAST {
	private:

		char operation;
		ExpressionAST *left;
		ExpressionAST *right;

	public:

		OperationAST(const Coordinate &coordinate, char operation, ExpressionAST *left, ExpressionAST *right);
		~OperationAST() override;

		char getOperation() const;
		ExpressionAST *getLeft() const;
		ExpressionAST *getRight() const;
	};

	/**
	 * Represents a value by a real number.
	 */
	class ValueAST : public ExpressionAST {
	private:

		double value;

	public:

		ValueAST(const Coordinate &coordinate, double value);

		double getValue() const;
	};

	/**
	 * Represents a constant by it's name.
	 */
	class ConstantAST : public ExpressionAST {
	private:

		std::string name;

	public:

		ConstantAST(const Coordinate &coordinate, const std::string &name);

		const std::string &getName() const;
	};

	/**
	 * Represents a function by it's name and it's parameter
	 * as an expression.
	 */
	class FunctionAST : public ExpressionAST {
	private:

		std::string name;
		ExpressionAST *parameter;

	public:

		FunctionAST(const Coordinate &coordinate, const std::string &name, ExpressionAST *parameter);

		const std::string &getName() const;
		const ExpressionAST *getParameter() const;
	};



	/**
	 * Represents a gate call by it's name, it's parameters as expressions
	 * and it's arguments as quantum registers
	 */
	class GateAST : public AST {
	private:

		std::string name;
		std::vector<ExpressionAST *> parameters;
		std::vector<QRegAST *> arguments;

	public:

		GateAST(const Coordinate &coordinate, const std::string &name, const std::vector<ExpressionAST *> &parameters, const std::vector<QRegAST *> &arguments);
		~GateAST() override;

		const std::string &getName() const;
		const std::vector<ExpressionAST *> &getParameters() const;
		const std::vector<QRegAST *> &getArguments() const;
	};

	/**
	 * Represents a barrier by the quantum registers it guards.
	 */
	class BarrierAST : public AST {
	private:

		std::vector<QRegAST *> arguments;

	public:

		BarrierAST(const Coordinate &coordinate, const std::vector<QRegAST *> &qreg);
		~BarrierAST() override;

		const std::vector<QRegAST *> &getArguments() const;
	};

	/**
	 * Represents a reset command by the quantum register it resets.
	 */
	class ResetAST : public AST {
	private:

		QRegAST *target;

	public:

		ResetAST(const Coordinate &coordinate, QRegAST *qreg);
		~ResetAST() override;

		QRegAST *getTarget() const;
	};

	/**
	 * Represents a measure command by the quantum register it measures
	 * and the target register as a real register.
	 */
	class MeasureAST : public AST {
	private:

		QRegAST *source;
		CRegAST *target;

	public:

		MeasureAST(const Coordinate &coordinate, QRegAST *qreg, CRegAST *creg);
		~MeasureAST() override;

		QRegAST *getSource() const;
		CRegAST *getTarget() const;
	};

	/**
	 * Represents a condition by the real register it checks against it's criteria
	 * and the command that it guards.
	 */
	class ConditionAST : public AST {
	private:

		CRegAST *reg;
		unsigned long criteria;
		AST *command;

	public:

		ConditionAST(const Coordinate &coordinate, CRegAST *creg, unsigned long integer, AST *command);
		~ConditionAST() override;

		CRegAST *getReg() const;
		unsigned long getCriteria() const;
		AST *getCommand() const;
	};
} }

using namespace ast::asts;


#endif //QUANTUMSIMULATOR_AST_H
