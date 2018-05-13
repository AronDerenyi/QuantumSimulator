#include "Program.h"

Program::Program(unsigned long bitCount, unsigned long qubitCount,
                 const std::map<std::string, std::vector<unsigned long>> &registerMap,
                 const std::vector<Instruction *> &instructions) :
		bitCount(bitCount), qubitCount(qubitCount), registerMap(registerMap), instructions(instructions) {

	programCounter = 0;
	executionCount = 0;

	results = new int[1 << bitCount];
	std::fill(results, results + (1 << bitCount), 0);
}

Program::~Program() {
	for (Instruction *instruction : instructions) delete instruction;
	delete[] results;
}

void Program::print(bool qe) {
	unsigned long comment = 0;
	for (unsigned long i = 0; i < instructions.size(); i++) {
		if (comment > 0) {
			comment--;
			std::cout << "// ";
		}
		comment = instructions[i]->print(std::cout, qe);
	}
}

void Program::execute() {
	Environment env(bitCount, qubitCount);

	for (programCounter = 0; programCounter < instructions.size(); programCounter++) {
		programCounter += instructions[programCounter]->execute(env);
	}

	int index = 0;
	for (unsigned long i = 0; i < bitCount; i++) {
		index += env.getBit(i) << i;
	}
	results[index]++;
	executionCount++;
}

void Program::printResults() {
	if (executionCount == 0) return;

	for (unsigned long regState = 0; regState < 1ul << bitCount; regState++) {
		if (results[regState] == 0) continue;
		double chance = (double) results[regState] / executionCount;
		for (std::pair<std::string, std::vector<unsigned long>> reg : registerMap) {
			std::cout << reg.first << "[";
			for (unsigned long i = 0; i < reg.second.size(); i++) {
				unsigned long index = reg.second.size() - i - 1;
				std::cout << ((regState >> reg.second[index]) & 1ul);
			}
			std::cout << "] ";
		}
		std::cout << ": " << chance << std::endl;
	}
}
