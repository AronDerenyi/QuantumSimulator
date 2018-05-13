#ifndef QUANTUMSIMULATOR_INTERPRETER_H
#define QUANTUMSIMULATOR_INTERPRETER_H


#include <vector>
#include <map>
#include <stack>
#include <random>
#include <cfloat>
#include "../ast/AST.h"
#include "Instruction.h"

namespace compiler {

	/**
	 * This is the main program that holds the instructions and
	 * the results of their executions.
	 */
	class Program {
	private:

		unsigned long programCounter;
		unsigned long executionCount;

		unsigned long bitCount;
		unsigned long qubitCount;

		std::map<std::string, std::vector<unsigned long>> registerMap;
		std::vector<Instruction *> instructions;
		int *results;

	public:

		/**
		 * Creates a new program from the given number of real bits and
		 * the number of quantum bits, the map grouping the real bits into registers
		 * and the instructions themselves.
		 *
		 * @param bitCount The number of real bits
		 * @param qubitCount The number of quantum bits
		 * @param registerMap The bit to register map
		 * @param instructions The instructions
		 */
		Program(unsigned long bitCount, unsigned long qubitCount,
		        const std::map<std::string, std::vector<unsigned long>> &registerMap,
		        const std::vector<Instruction *> &instructions);

		/**
		 * Deletes the instructions and the results.
		 */
		~Program();

		/**
		 * Prints the instructions one by one. If "qe" is true, then the instructions
		 * are printed in an ibm quantum experience friendly way, so that it can be
		 * tested on a real quantum computer.
		 * Only used for debugging!
		 *
		 * @param qe Enables quantum experience friendly mode
		 */
		void print(bool qe = false);

		/**
		 * Executes the instructions and stores the results.
		 */
		void execute();

		/**
		 * Prints the interpreted execution results grouped by the registers.
		 */
		void printResults();
	};
}

using namespace compiler;


#endif //QUANTUMSIMULATOR_INTERPRETER_H
