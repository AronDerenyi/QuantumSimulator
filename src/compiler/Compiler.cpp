#include "Compiler.h"

Compiler::Exception::Exception(const Coordinate &coordinate, const char *message) noexcept :
		runtime_error("Error in \"" + coordinate.getFile() +
		              "\" at line " + std::to_string(coordinate.getLine()) +
		              " column " + std::to_string(coordinate.getColumn()) +
		              ":\n" + message){

}



Compiler::GateScope::GateScope(const std::map<std::string, unsigned long> &localQubitIdMap,
                               const std::map<std::string, double> &localConstants) :
		localQubitIdMap(localQubitIdMap), localConstants(localConstants) {

}

unsigned long Compiler::GateScope::getLocalQubitId(QRegAST *qreg) const {
	if (qreg->isIndexed()) throw Exception(qreg->getCoordinate(), "Local qubit can't be indexed");

	unsigned long id;
	try {
		id = localQubitIdMap.at(qreg->getName());
	} catch (std::out_of_range &e) {
		throw Exception(qreg->getCoordinate(), "Local qubit doesn't exist");
	}

	return id;
}

const std::map<std::string, double> &Compiler::GateScope::getLocalConstants() const {
	return localConstants;
}



Program *Compiler::compile(ProgramAST *program) {
	Compiler compiler;
	std::vector<Instruction *> instructions = compiler.compileProgram(program);

	return new Program(
			compiler.bitCount,
			compiler.qubitCount,
			compiler.cregIdMap,
			instructions
	);
}



std::vector<Instruction *> Compiler::compileProgram(const ProgramAST *program) {
	if (program->getVersion() != 2.0) throw Exception(program->getCoordinate(), "Invalid program version");
	return compileCommands(program->getCommands());
}

std::vector<Instruction *> Compiler::compileInclude(const IncludeAST *include) {
	return compileCommands(include->getCommands());
}

std::vector<Instruction *> Compiler::compileCommands(const std::vector<AST *> &commands) {
	std::vector<Instruction *> instructions;

	for (AST *command : commands) {
		std::vector<Instruction *> compiled;
		switch (command->getType()) {
			case AST::INCLUDE:
				compiled = compileInclude((IncludeAST *) command);
				break;
			case AST::CREG_DECLARATION:
				compileCRegDeclaration((CRegDeclarationAST *) command);
				break;
			case AST::QREG_DECLARATION:
				compileQRegDeclaration((QRegDeclarationAST *) command);
				break;
			case AST::GATE_DECLARATION:
				compileGateDeclaration((GateDeclarationAST *) command);
				break;
			case AST::OPAQUE_DECLARATION:
				compileOpaqueDeclaration((OpaqueDeclarationAST *) command);
				break;
			case AST::GATE:
				compiled = compileGate((GateAST *) command);
				break;
			case AST::RESET:
				compiled = compileReset((ResetAST *) command);
				break;
			case AST::BARRIER:
				compiled = compileBarrier((BarrierAST *) command);
				break;
			case AST::MEASURE:
				compiled = compileMeasure((MeasureAST *) command);
				break;
			case AST::CONDITION:
				compiled = compileCondition((ConditionAST *) command);
				break;
			default:
				throw Exception(command->getCoordinate(), "Incorrect command");
		}
		instructions.insert(instructions.end(), compiled.begin(), compiled.end());
	}
	return instructions;
}



void Compiler::compileCRegDeclaration(const CRegDeclarationAST *cregDeclaration) {
	std::vector<unsigned long> creg;
	for (int i = 0; i < cregDeclaration->getSize(); i++) creg.push_back(bitCount++);
	cregIdMap[cregDeclaration->getName()] = creg;
}

void Compiler::compileQRegDeclaration(const QRegDeclarationAST *qregDeclaration) {
	std::vector<unsigned long> qreg;
	for (int i = 0; i < qregDeclaration->getSize(); i++) qreg.push_back(qubitCount++);
	qregIdMap[qregDeclaration->getName()] = qreg;
}



void Compiler::compileGateDeclaration(const GateDeclarationAST *gateDeclaration) {
	std::string name = gateDeclaration->getName();
	for (std::string::const_iterator i = name.begin(); i != name.end(); i++) if (isalpha(*i) && isupper(*i)) {
		throw Exception(gateDeclaration->getCoordinate(), "Only built in gates can contain uppercase characters");
	}
	if (gates.find(gateDeclaration->getName()) != gates.end()) {
		throw Exception(gateDeclaration->getCoordinate(), "Duplicate gate declaration");
	}

	for (AST *command : gateDeclaration->getCommands()) {
		GateAST *gate;
		switch (command->getType()) {
			case AST::GATE:
				gate = (GateAST *) command;
				if (gate->getName() != "U" && gate->getName() != "CX" && gates.find(gate->getName()) == gates.end()) {
					throw Exception(command->getCoordinate(), "Gate not declared yet");
				}
				break;
			case AST::BARRIER:
				break;
			default:
				throw Exception(command->getCoordinate(), "Incorrect command");
		}
	}

	gates[gateDeclaration->getName()] = gateDeclaration;
}

void Compiler::compileOpaqueDeclaration(const OpaqueDeclarationAST *opaqueDeclaration) {
	throw Exception(opaqueDeclaration->getCoordinate(), "Opaques are not supported");
}



std::vector<Instruction *> Compiler::compileGate(const GateAST *gate, bool nested, const GateScope &scope) {
	std::vector<Instruction *> instructions;

	const GateDeclarationAST *declaration;
	unsigned long parameterCount;
	unsigned long argumentCount;
	bool native;

	try {
		declaration = gates.at(gate->getName());
		parameterCount = declaration->getParameters().size();
		argumentCount = declaration->getArguments().size();
		native = false;
	} catch (std::out_of_range &e) {
		if (gate->getName() == "U") {
			parameterCount = 3;
			argumentCount = 1;
			native = true;
		} else if (gate->getName() == "CX") {
			parameterCount = 0;
			argumentCount = 2;
			native = true;
		} else {
			throw Exception(gate->getCoordinate(), "Gate not declared yet");
		}
	}

	if (gate->getParameters().size() < parameterCount) {
		throw Exception(gate->getCoordinate(), "Too few parameters");
	}
	if (gate->getParameters().size() > parameterCount) {
		throw Exception(gate->getCoordinate(), "Too many parameters");
	}

	if (gate->getArguments().size() < argumentCount) {
		throw Exception(gate->getCoordinate(), "Too few arguments");
	}
	if (gate->getArguments().size() > argumentCount) {
		throw Exception(gate->getCoordinate(), "Too many arguments");
	}

	std::vector<double> parameterValues;
	for (ExpressionAST *parameter : gate->getParameters()) {
		double value = getValue(parameter, scope.getLocalConstants());
		parameterValues.push_back(value);
	}

	std::vector<std::vector<unsigned long>> argumentIds;
	for (QRegAST *argument : gate->getArguments()) {
		std::vector<unsigned long> ids;
		if (nested) {
			ids.push_back(scope.getLocalQubitId(argument));
		} else {
			ids = getQRegIds(argument);
		}
		argumentIds.push_back(ids);
	}

	unsigned long iterations = 1;
	for (const std::vector<unsigned long> &ids : argumentIds) {
		if (ids.size() > 1 && iterations > 1 && ids.size() != iterations) {
			throw Exception(gate->getCoordinate(), "Register size is incompatible with the other registers");
		}
		if (ids.size() > iterations) iterations = ids.size();
	}

	for (unsigned long i = 0; i < iterations; i++) {
		std::vector<unsigned long> arguments;
		for (const std::vector<unsigned long> &ids : argumentIds) {
			unsigned long index = 0;
			if (ids.size() > 1) index = i;
			arguments.push_back(ids[index]);
		}

		if (native) {
			if (gate->getName() == "U") {
				instructions.push_back(new U(parameterValues[0], parameterValues[1], parameterValues[2], arguments[0]));
			} else if (gate->getName() == "CX") {
				instructions.push_back(new CX(arguments[0], arguments[1]));
			}
		} else {
			std::map<std::string, double> gateParameters;
			std::map<std::string, unsigned long> gateArguments;

			for (unsigned long i = 0; i < parameterValues.size(); i++) {
				gateParameters[declaration->getParameters()[i]] = parameterValues[i];
			}
			for (unsigned long i = 0; i < arguments.size(); i++) {
				gateArguments[declaration->getArguments()[i]] = arguments[i];
			}

			for (AST *command : declaration->getCommands()) {
				std::vector<Instruction *> compiled;
				switch (command->getType()) {
					case AST::GATE:
						compiled = compileGate(((GateAST *) command), true, GateScope(gateArguments, gateParameters));
						break;
					case AST::BARRIER:
						compiled = compileBarrier(((BarrierAST *) command), true, GateScope(gateArguments, gateParameters));
						break;
					default:
						throw Exception(command->getCoordinate(), "Incorrect command");
				}
				instructions.insert(instructions.end(), compiled.begin(), compiled.end());
			}
		}
	}

	return instructions;
}

std::vector<Instruction *> Compiler::compileBarrier(const BarrierAST *barrier, bool nested, const GateScope &scope) {
	std::vector<Instruction *> instructions;
	std::vector<unsigned long> arguments;

	for (QRegAST *qreg : barrier->getArguments()) {
		if (nested) {
			arguments.push_back(scope.getLocalQubitId(qreg));
		} else {
			std::vector<unsigned long> ids = getQRegIds(qreg);
			arguments.insert(arguments.end(), ids.begin(), ids.end());
		}
	}

	for (unsigned long argument : arguments) {
		instructions.push_back(new Barrier(argument));
	}

	return instructions;
}

std::vector<Instruction *> Compiler::compileReset(const ResetAST *reset) {
	std::vector<Instruction *> instructions;
	std::vector<unsigned long> qregIds = getQRegIds(reset->getTarget());

	for (unsigned long qreg : qregIds) {
		instructions.push_back(new Reset(qreg));
	}

	return instructions;
}

std::vector<Instruction *> Compiler::compileMeasure(const MeasureAST *measure) {
	std::vector<Instruction *> instructions;
	std::vector<unsigned long> qregIds = getQRegIds(measure->getSource());
	std::vector<unsigned long> cregIds = getCRegIds(measure->getTarget());

	if (qregIds.size() != cregIds.size()) {
		throw Exception(measure->getCoordinate(), "Register sizes don't match");
	}

	for (int i = 0; i < qregIds.size(); i++) {
		instructions.push_back(new Measure(qregIds[i], cregIds[i]));
	}

	return instructions;
}

std::vector<Instruction *> Compiler::compileCondition(const ConditionAST *condition) {
	std::vector<Instruction *> instructions;
	std::vector<unsigned long> bitIds = getCRegIds(condition->getReg());

	AST *command = condition->getCommand();
	std::vector<Instruction *> compiled;
	switch (command->getType()) {
		case AST::GATE:
			compiled = compileGate((GateAST *) command);
			break;
		case AST::RESET:
			compiled = compileReset((ResetAST *) command);
			break;
		case AST::MEASURE:
			compiled = compileMeasure((MeasureAST *) command);
			break;
		default:
			throw Exception(command->getCoordinate(), "Incorrect command");
	}

	instructions.push_back(new Condition(
			bitIds,
			condition->getCriteria(),
			compiled.size()
	));
	instructions.insert(instructions.end(), compiled.begin(), compiled.end());

	return instructions;
}



std::vector<unsigned long> Compiler::getCRegIds(CRegAST *creg) {
	std::vector<unsigned long> ids;

	try {
		ids = cregIdMap.at(creg->getName());
	} catch (std::out_of_range &e) {
		throw Exception(creg->getCoordinate(), "Register not declared yet");
	}

	if (creg->isIndexed()) {
		unsigned long id;
		try {
			id = ids.at(creg->getIndex());
		} catch (std::out_of_range &e) {
			throw Exception(creg->getCoordinate(), "Bit index is out of range");
		}

		ids.clear();
		ids.push_back(id);
	}

	return ids;
}

std::vector<unsigned long> Compiler::getQRegIds(QRegAST *qreg) {
	std::vector<unsigned long> ids;

	try {
		ids = qregIdMap.at(qreg->getName());
	} catch (std::out_of_range &e) {
		throw Exception(qreg->getCoordinate(), "Register not declared yet");
	}

	if (qreg->isIndexed()) {
		unsigned long id;
		try {
			id = ids.at(qreg->getIndex());
		} catch (std::out_of_range &e) {
			throw Exception(qreg->getCoordinate(), "Qubit index is out of range");
		}

		ids.clear();
		ids.push_back(id);
	}

	return ids;
}



double Compiler::getValue(const ExpressionAST *expression, const std::map<std::string, double> &constants) {
	OperationAST *operation;
	ValueAST *value;
	ConstantAST *constant;
	FunctionAST *function;

	switch (expression->getExpressionType()) {
		case ExpressionAST::OPERATION:
			operation = (OperationAST *) expression;
			switch (operation->getOperation()) {
				case '+':
					return getValue(operation->getLeft(), constants) + getValue(operation->getRight(), constants);
				case '-':
					return getValue(operation->getLeft(), constants) - getValue(operation->getRight(), constants);
				case '*':
					return getValue(operation->getLeft(), constants) * getValue(operation->getRight(), constants);
				case '/':
					return getValue(operation->getLeft(), constants) / getValue(operation->getRight(), constants);
				case '^':
					return pow(getValue(operation->getLeft(), constants), getValue(operation->getRight(), constants));
				default:
					throw Exception(expression->getCoordinate(), "Unknown operation");
			}
		case ExpressionAST::VALUE:
			value = (ValueAST *) expression;
			return value->getValue();
		case ExpressionAST::CONSTANT:
			constant = (ConstantAST *) expression;
			try {
				return constants.at(constant->getName());
			} catch (std::out_of_range &e) {
				if (constant->getName() == "pi") {
					return M_PI;
				} else {
					throw Exception(expression->getCoordinate(), "Unknown constant");
				}
			}
		case ExpressionAST::FUNCTION:
			function = (FunctionAST *) expression;
			if (function->getName() == "sin") {
				return sin(getValue(function->getParameter(), constants));
			} else if (function->getName() == "cos") {
				return cos(getValue(function->getParameter(), constants));
			} else if (function->getName() == "tan") {
				return tan(getValue(function->getParameter(), constants));
			} else if (function->getName() == "exp") {
				return exp(getValue(function->getParameter(), constants));
			} else if (function->getName() == "ln") {
				return log(getValue(function->getParameter(), constants));
			} else if (function->getName() == "sqrt") {
				return sqrt(getValue(function->getParameter(), constants));
			} else {
				throw Exception(expression->getCoordinate(), "Unknown function");
			}
		default:
			throw Exception(expression->getCoordinate(), "Unknown expression");
	}
}
