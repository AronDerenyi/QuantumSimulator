#include "AST.h"

// BASE

AST::AST(const Coordinate &coordinate, const AST::Type type) : coordinate(coordinate), type(type) {

}

AST::~AST() = default;

const Coordinate &AST::getCoordinate() const {
	return coordinate;
}

AST::Type AST::getType() const {
	return type;
}

// PROGRAM

ProgramAST::ProgramAST(const Coordinate &coordinate, const double version, const std::vector<AST *> &commands) :
		AST(coordinate, PROGRAM), version(version), commands(commands) {

}

ProgramAST::~ProgramAST() {
	for (AST *command : commands) delete command;
}

double ProgramAST::getVersion() const {
	return version;
}

const std::vector<AST *> &ProgramAST::getCommands() const {
	return commands;
}

// INCLUDE

IncludeAST::IncludeAST(const Coordinate &coordinate, std::vector<AST *> &commands) :
		AST(coordinate, INCLUDE), commands(commands) {
}

IncludeAST::~IncludeAST() {
	for (AST *command : commands) delete command;
}

const std::vector<AST *> &IncludeAST::getCommands() const {
	return commands;
}

// CREG

CRegAST::CRegAST(const Coordinate &coordinate, const std::string &name, bool indexed, unsigned long index) :
		AST(coordinate, CREG), name(name), indexed(indexed), index(index) {

}

const std::string &CRegAST::getName() const {
	return name;
}

bool CRegAST::isIndexed() const {
	return indexed;
}

unsigned long CRegAST::getIndex() const {
	return index;
}

// CREG DECLARATION

CRegDeclarationAST::CRegDeclarationAST(const Coordinate &coordinate, const std::string &name, unsigned long size) :
		AST(coordinate, CREG_DECLARATION), name(name), size(size) {

}

const std::string &CRegDeclarationAST::getName() const {
	return name;
}

unsigned long CRegDeclarationAST::getSize() const {
	return size;
}

// QREG

QRegAST::QRegAST(const Coordinate &coordinate, const std::string &name, bool indexed, unsigned long index) :
		AST(coordinate, QREG), name(name), indexed(indexed), index(index) {

}

const std::string &QRegAST::getName() const {
	return name;
}

bool QRegAST::isIndexed() const {
	return indexed;
}

unsigned long QRegAST::getIndex() const {
	return index;
}

// QREG DECLARATION

QRegDeclarationAST::QRegDeclarationAST(const Coordinate &coordinate, const std::string &name, unsigned long size) :
		AST(coordinate, QREG_DECLARATION), name(name), size(size) {

}

const std::string &QRegDeclarationAST::getName() const {
	return name;
}

unsigned long QRegDeclarationAST::getSize() const {
	return size;
}

// GATE DECLARATION

GateDeclarationAST::GateDeclarationAST(const Coordinate &coordinate, const std::string &name,
                                       const std::vector<std::string> &parameters,
                                       const std::vector<std::string> &arguments,
                                       const std::vector<AST *> &commands) :
		AST(coordinate, GATE_DECLARATION), name(name), parameters(parameters), arguments(arguments), commands(commands) {

}

GateDeclarationAST::~GateDeclarationAST() {
	for (AST *command : commands) delete command;
}

const std::string &GateDeclarationAST::getName() const {
	return name;
}

const std::vector<std::string> &GateDeclarationAST::getParameters() const {
	return parameters;
}

const std::vector<std::string> &GateDeclarationAST::getArguments() const {
	return arguments;
}

const std::vector<AST *> &GateDeclarationAST::getCommands() const {
	return commands;
}

// OPAQUE DECLARATION

OpaqueDeclarationAST::OpaqueDeclarationAST(const Coordinate &coordinate, const std::string &name,
                                           const std::vector<std::string> &parameters,
                                           const std::vector<std::string> &arguments) :
		AST(coordinate, OPAQUE_DECLARATION), name(name), parameters(parameters), arguments(arguments) {

}

const std::string &OpaqueDeclarationAST::getName() const {
	return name;
}

const std::vector<std::string> &OpaqueDeclarationAST::getParameters() const {
	return parameters;
}

const std::vector<std::string> &OpaqueDeclarationAST::getArguments() const {
	return arguments;
}

// EXPRESSION

ExpressionAST::ExpressionAST(const Coordinate &coordinate, const ExpressionAST::ExpressionType expressionType) :
		AST(coordinate, EXPRESSION), expressionType(expressionType) {

}

ExpressionAST::ExpressionType ExpressionAST::getExpressionType() const {
	return expressionType;
}

// OPERATION

OperationAST::OperationAST(const Coordinate &coordinate, const char operation,
                           ExpressionAST *left, ExpressionAST *right) :
		ExpressionAST(coordinate, OPERATION), operation(operation), left(left), right(right) {

}

OperationAST::~OperationAST() {
	delete left;
	delete right;
}

char OperationAST::getOperation() const {
	return operation;
}

ExpressionAST *OperationAST::getLeft() const {
	return left;
}

ExpressionAST *OperationAST::getRight() const {
	return right;
}

// VALUE

ValueAST::ValueAST(const Coordinate &coordinate, const double value) :
		ExpressionAST(coordinate, VALUE), value(value) {

}

double ValueAST::getValue() const {
	return value;
}

// CONSTANT

ConstantAST::ConstantAST(const Coordinate &coordinate, const std::string &name) :
		ExpressionAST(coordinate, CONSTANT), name(name) {

}

const std::string &ConstantAST::getName() const {
	return name;
}

// FUNCTION

FunctionAST::FunctionAST(const Coordinate &coordinate, const std::string &name, ExpressionAST *parameter) :
		ExpressionAST(coordinate, FUNCTION), name(name), parameter(parameter) {

}

const std::string &FunctionAST::getName() const {
	return name;
}

const ExpressionAST *FunctionAST::getParameter() const {
	return parameter;
}

// GATE

GateAST::GateAST(const Coordinate &coordinate, const std::string &name,
                 const std::vector<ExpressionAST *> &parameters,
                 const std::vector<QRegAST *> &arguments) :
		AST(coordinate, GATE), name(name), parameters(parameters), arguments(arguments) {

}

GateAST::~GateAST() {
	for (ExpressionAST *expression : parameters) delete expression;
	for (QRegAST *qreg : arguments) delete qreg;
}

const std::string &GateAST::getName() const {
	return name;
}

const std::vector<ExpressionAST *> &GateAST::getParameters() const {
	return parameters;
}

const std::vector<QRegAST *> &GateAST::getArguments() const {
	return arguments;
}

// BARRIER

BarrierAST::BarrierAST(const Coordinate &coordinate, const std::vector<QRegAST *> &arguments) :
		AST(coordinate, BARRIER), arguments(arguments) {

}

BarrierAST::~BarrierAST() {
	for (QRegAST *qreg : arguments) delete qreg;
}

const std::vector<QRegAST *> &BarrierAST::getArguments() const {
	return arguments;
}

// RESET

ResetAST::ResetAST(const Coordinate &coordinate, QRegAST *target) :
		AST(coordinate, RESET), target(target) {

}

ResetAST::~ResetAST() {
	delete target;
}

QRegAST *ResetAST::getTarget() const {
	return target;
}

// MEASURE

MeasureAST::MeasureAST(const Coordinate &coordinate, QRegAST *source, CRegAST *target) :
		AST(coordinate, MEASURE), source(source), target(target) {

}

MeasureAST::~MeasureAST() {
	delete source;
	delete target;
}

QRegAST *MeasureAST::getSource() const {
	return source;
}

CRegAST *MeasureAST::getTarget() const {
	return target;
}

// CONDITION

ConditionAST::ConditionAST(const Coordinate &coordinate, CRegAST *reg, unsigned long criteria, AST *command) :
		AST(coordinate, CONDITION), reg(reg), criteria(criteria), command(command) {

}

ConditionAST::~ConditionAST() {
	delete reg;
	delete command;
}

CRegAST *ConditionAST::getReg() const {
	return reg;
}

unsigned long ConditionAST::getCriteria() const {
	return criteria;
}

AST *ConditionAST::getCommand() const {
	return command;
}
