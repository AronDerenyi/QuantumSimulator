#ifndef QUANTUMSIMULATOR_COMPILER_H
#define QUANTUMSIMULATOR_COMPILER_H


#include <map>
#include "../ast/AST.h"
#include "../math/Environment.h"
#include "Instruction.h"
#include "Program.h"

namespace compiler {

	/**
	 * Creates a program from an abstract syntax tree.
	 */
	class Compiler {
	public:

		/**
		 * A runtime error, thrown when an error happens during compilation.
		 */
		class Exception : public std::runtime_error {
		public:

			Exception(const Coordinate &coordinate, const char* message) noexcept;
		};

	private:

		/**
		 * This class represents a gate's scope meaning,
		 * it holds the gate's parameters' values and
		 * the gate's argument qubits' ids.
		 */
		class GateScope {
		private:

			std::map<std::string, unsigned long> localQubitIdMap;
			std::map<std::string, double> localConstants;

		public:

			explicit GateScope(
					const std::map<std::string, unsigned long> &localQubitIdMap = std::map<std::string, unsigned long>(),
			        const std::map<std::string, double> &localConstants = std::map<std::string, double>());

			/**
			 * Returns the id of a local qubit.
			 *
			 * @param qreg The qreg representing a qubit
			 * @return The id of a qubit
			 */
			unsigned long getLocalQubitId(QRegAST *qreg) const;

			/**
			 * Returns a map associating a parameter's name
			 * to the parameter's value.
			 *
			 * @return The map of parameters and their values
			 */
			const std::map<std::string, double> &getLocalConstants() const;
		};

		unsigned long bitCount = 0;
		unsigned long qubitCount = 0;
		std::map<std::string, std::vector<unsigned long>> cregIdMap;
		std::map<std::string, std::vector<unsigned long>> qregIdMap;
		std::map<std::string, const GateDeclarationAST *> gates;

		/**
		 * Compiles the given program AST's children into a vector of instructions,
		 * and checks for the program version.
		 *
		 * @param program The program AST
		 * @return The compiled instructions
		 */
		std::vector<Instruction *> compileProgram(const ProgramAST *program);

		/**
		 * Compiles the given include AST's children into a vector of instructions.
		 *
		 * @param include The include AST
		 * @return The compiled instructions
		 */
		std::vector<Instruction *> compileInclude(const IncludeAST *include);

		/**
		 * Compiles the given ASTs into a vector of instructions
		 *
		 * @param commands The ASTs
		 * @return The compiled instructions
		 */
		std::vector<Instruction *> compileCommands(const std::vector<AST *> &commands);

		/**
		 * Compiles a creg declaration AST meaning, generates new ids
		 * for the register's bits, and adds them to the creg map.
		 *
		 * @param cregDeclaration The declaration AST
		 */
		void compileCRegDeclaration(const CRegDeclarationAST *cregDeclaration);

		/**
		 * Compiles a qreg declaration AST meaning, generates new ids
		 * for the register's qubits, and adds them to the qreg map.
		 *
		 * @param qregDeclaration The declaration AST
		 */
		void compileQRegDeclaration(const QRegDeclarationAST *qregDeclaration);

		/**
		 * Compiles a gate declaration AST meaning, check for validity
		 * and adds it to the gates map.
		 *
		 * @param gateDeclaration The declaration AST
		 */
		void compileGateDeclaration(const GateDeclarationAST *gateDeclaration);

		/**
		 * Compiles a gate declaration AST, but since an opaque gate
		 * requires a physical implementation, this simulation
		 * throws an exception.
		 *
		 * @param opaqueDeclaration The declaration AST
		 */
		void compileOpaqueDeclaration(const OpaqueDeclarationAST *opaqueDeclaration);

		/**
		 * Compiles a gate AST into a vector of instructions.
		 *
		 * @param gate The gate AST
		 * @param nested True if the gate is called from another gate
		 * @param scope The gate's scope from which the gate is called
		 * @return The compiled instructions
		 */
		std::vector<Instruction *> compileGate(const GateAST *gate, bool nested = false, const GateScope &scope = GateScope());

		/**
		 * Compiles a barrier AST into a an instruction.
		 *
		 * @param barrier The barrier AST
		 * @param nested True if the barrier is called from another gate
		 * @param scope The gate's scope from which the barrier is called
		 * @return The compiled instruction (a vector with only one item)
		 */
		std::vector<Instruction *> compileBarrier(const BarrierAST *barrier, bool nested = false, const GateScope &scope = GateScope());

		/**
		 * Compiles a reset AST into a an instruction.
		 *
		 * @param reset The reset AST
		 * @return The compiled instruction (a vector with only one item)
		 */
		std::vector<Instruction *> compileReset(const ResetAST *reset);

		/**
		 * Compiles a measure AST into a an instruction.
		 *
		 * @param measure The measure AST
		 * @return The compiled instruction (a vector with only one item)
		 */
		std::vector<Instruction *> compileMeasure(const MeasureAST *measure);

		/**
		 * Compiles a condition AST into a an instruction.
		 *
		 * @param condition The condition AST
		 * @return The compiled instruction (a vector with only one item)
		 */
		std::vector<Instruction *> compileCondition(const ConditionAST *condition);

		/**
		 * Returns the given creg AST's bits' ids
		 *
		 * @param creg The creg AST
		 * @return The creg's ids
		 */
		std::vector<unsigned long> getCRegIds(CRegAST *creg);

		/**
		 * Returns the given qreg AST's qubits' ids
		 *
		 * @param qreg The qreg AST
		 * @return The qreg's ids
		 */
		std::vector<unsigned long> getQRegIds(QRegAST *qreg);

		/**
		 * Returns the interpreted value of the given expression
		 * by substituting the constants from the given name-value map.
		 *
		 * @param expression The expression
		 * @param constants The map associating the constant names to real values
		 * @return The interpreted value of the expression
		 */
		double getValue(const ExpressionAST *expression, const std::map<std::string, double> &constants);

	public:

		/**
		 * Compiles the given abstract syntax tree (where the root node is a ProgramAST)
		 * to a program object.
		 *
		 * @param program The root node of an abstract syntax tree
		 * @return The program containing the compiled instructions
		 */
		static Program *compile(ProgramAST *program);
	};
}

using namespace compiler;


#endif //QUANTUMSIMULATOR_COMPILER_H
