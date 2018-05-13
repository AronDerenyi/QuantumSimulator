#ifndef QUANTUMSIMULATOR_PRINTER_H
#define QUANTUMSIMULATOR_PRINTER_H


#include "AST.h"

namespace ast {

	/**
	 * Prints all the abstract syntax tree nodes.
	 * Only used for debugging!
	 */
	class Printer {
	private:

		static std::string in;

		static void indent();
		static void exdent();

		static void printProgram(const ProgramAST *program);
		static void printInclude(const IncludeAST *include);

		static void printCReg(const CRegAST *creg);
		static void printCRegDeclaration(const CRegDeclarationAST *cregDeclaration);

		static void printQReg(const QRegAST *qreg);
		static void printQRegDeclaration(const QRegDeclarationAST *qregDeclaration);

		static void printGateDeclaration(const GateDeclarationAST *gateDeclaration);
		static void printOpaqueDeclaration(const OpaqueDeclarationAST *opaqueDeclaration);

		static void printExpression(const ExpressionAST *expression);
		static void printExpressionOperation(const OperationAST *operation);
		static void printExpressionValue(const ValueAST *value);
		static void printExpressionConstant(const ConstantAST *constant);
		static void printExpressionFunction(const FunctionAST *function);

		static void printGate(const GateAST *gate);
		static void printBarrier(const BarrierAST *barrier);
		static void printReset(const ResetAST *reset);
		static void printMeasure(const MeasureAST *measure);
		static void printCondition(const ConditionAST *condition);

	public:

		static void print(const AST* ast);
	};
}

using namespace ast;


#endif //QUANTUMSIMULATOR_PRINTER_H
