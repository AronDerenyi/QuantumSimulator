#include "Printer.h"

std::string Printer::in;

void Printer::indent() {
	in += "  ";
}

void Printer::exdent() {
	in = in.substr(2);
}

void Printer::print(const AST *ast) {
	switch (ast->getType()) {
		case AST::PROGRAM:
			printProgram((ProgramAST*) ast);
			break;
		case AST::INCLUDE:
			printInclude((IncludeAST*) ast);
			break;

		case AST::CREG:
			printCReg((CRegAST*) ast);
			break;
		case AST::CREG_DECLARATION:
			printCRegDeclaration((CRegDeclarationAST*) ast);
			break;

		case AST::QREG:
			printQReg((QRegAST*) ast);
			break;
		case AST::QREG_DECLARATION:
			printQRegDeclaration((QRegDeclarationAST*) ast);
			break;

		case AST::GATE_DECLARATION:
			printGateDeclaration((GateDeclarationAST*) ast);
			break;
		case AST::OPAQUE_DECLARATION:
			printOpaqueDeclaration((OpaqueDeclarationAST*) ast);
			break;

		case AST::EXPRESSION:
			printExpression((ExpressionAST*) ast);
			break;

		case AST::GATE:
			printGate((GateAST*) ast);
			break;
		case AST::BARRIER:
			printBarrier((BarrierAST*) ast);
			break;
		case AST::RESET:
			printReset((ResetAST*) ast);
			break;
		case AST::MEASURE:
			printMeasure((MeasureAST*) ast);
			break;
		case AST::CONDITION:
			printCondition((ConditionAST*) ast);
			break;
	}
}

void Printer::printProgram(const ProgramAST *program) {
	std::cout << in << "program (" + program->getCoordinate().getFile() + ") {" << std::endl;
	indent();
	for (AST *command : program->getCommands()) print(command);
	exdent();
	std::cout << in << "}" << std::endl;
}
void Printer::printInclude(const IncludeAST *include) {
	std::cout << in << "include (" << include->getCommands()[0]->getCoordinate().getFile() << ") {" << std::endl;
	indent();
	for (AST *command : include->getCommands()) print(command);
	exdent();
	std::cout << in << "}" << std::endl;
}

void Printer::printCReg(const CRegAST *creg) {
	std::cout << in << "creg " << creg->getName();
	if (creg->isIndexed()) {
		std::cout << "[" << creg->getIndex() << "]";
	}
	std::cout << std::endl;
}
void Printer::printCRegDeclaration(const CRegDeclarationAST *cregDeclaration) {
	std::cout << in << "declare creg " << cregDeclaration->getName() << "[" << cregDeclaration->getSize() << "]" << std::endl;
}

void Printer::printQReg(const QRegAST *qreg) {
	std::cout << in << "qreg " << qreg->getName();
	if (qreg->isIndexed()) {
		std::cout << "[" << qreg->getIndex() << "]";
	}
	std::cout << std::endl;
}
void Printer::printQRegDeclaration(const QRegDeclarationAST *qregDeclaration) {
	std::cout << in << "declare qreg " << qregDeclaration->getName() << "[" << qregDeclaration->getSize() << "]" << std::endl;
}

void Printer::printGateDeclaration(const GateDeclarationAST *gateDeclaration) {
	std::cout << in << "declare gate " << gateDeclaration->getName() << "(" << std::endl;
	indent();
	for (const std::string &parameter : gateDeclaration->getParameters()) std::cout << in << parameter << std::endl;
	exdent();
	std::cout << in << ") (" << std::endl;
	indent();
	for (const std::string &argument : gateDeclaration->getArguments()) std::cout << in << argument << std::endl;
	exdent();
	std::cout << in << ") {" << std::endl;
	indent();
	for (AST *command : gateDeclaration->getCommands()) print(command);
	exdent();
	std::cout << in << "}" << std::endl;
}
void Printer::printOpaqueDeclaration(const OpaqueDeclarationAST *opaqueDeclaration) {
	std::cout << in << "declare opaque " << opaqueDeclaration->getName() << "(" << std::endl;
	indent();
	for (const std::string &parameter : opaqueDeclaration->getParameters()) std::cout << in << parameter << std::endl;
	exdent();
	std::cout << in << ") (" << std::endl;
	indent();
	for (const std::string &argument : opaqueDeclaration->getArguments()) std::cout << in << argument << std::endl;
	exdent();
	std::cout << in << ")" << std::endl;
}

void Printer::printExpression(const ExpressionAST *expression) {
	switch (expression->getExpressionType()) {
		case ExpressionAST::OPERATION:
			printExpressionOperation((OperationAST*) expression);
			break;
		case ExpressionAST::VALUE:
			printExpressionValue((ValueAST*) expression);
			break;
		case ExpressionAST::CONSTANT:
			printExpressionConstant((ConstantAST*) expression);
			break;
		case ExpressionAST::FUNCTION:
			printExpressionFunction((FunctionAST*) expression);
			break;
	}
}
void Printer::printExpressionOperation(const OperationAST *operation) {
	std::cout << in << "operation " << operation->getOperation() << "(" << std::endl;
	indent();
	print(operation->getLeft());
	print(operation->getRight());
	exdent();
	std::cout << in << ")" << std::endl;
}
void Printer::printExpressionValue(const ValueAST *value) {
	std::cout << in << "value " << value->getValue() << std::endl;
}
void Printer::printExpressionConstant(const ConstantAST *constant) {
	std::cout << in << "constant " << constant->getName() << std::endl;
}
void Printer::printExpressionFunction(const FunctionAST *function) {
	std::cout << in << "function " << function->getName() << "(" << std::endl;
	indent();
	print(function->getParameter());
	exdent();
	std::cout << in << ")" << std::endl;
}

void Printer::printGate(const GateAST *gate) {
	std::cout << in << "gate " << gate->getName() << "(" << std::endl;
	indent();
	for (ExpressionAST *expression : gate->getParameters()) print(expression);
	exdent();
	std::cout << in << ") (" << std::endl;
	indent();
	for (QRegAST *qreg : gate->getArguments()) print(qreg);
	exdent();
	std::cout << in << ")" << std::endl;
}
void Printer::printBarrier(const BarrierAST *barrier) {
	std::cout << in << "barrier " << "(" << std::endl;
	indent();
	for (QRegAST *qreg : barrier->getArguments()) print(qreg);
	exdent();
	std::cout << in << ")" << std::endl;
}
void Printer::printReset(const ResetAST *reset) {
	std::cout << in << "reset " << "(" << std::endl;
	indent();
	print(reset->getTarget());
	exdent();
	std::cout << in << ")" << std::endl;
}
void Printer::printMeasure(const MeasureAST *measure) {
	std::cout << in << "measure " << "(" << std::endl;
	indent();
	print(measure->getSource());
	print(measure->getTarget());
	exdent();
	std::cout << in << ")" << std::endl;
}
void Printer::printCondition(const ConditionAST *condition) {
	std::cout << in << "if " << "(" << std::endl;
	indent();
	print(condition->getReg());
	std::cout << in << condition->getCriteria() << std::endl;
	exdent();
	std::cout << in << ") {" << std::endl;
	indent();
	print(condition->getCommand());
	exdent();
	std::cout << in << "}" << std::endl;
}
