#ifndef QUANTUMSIMULATOR_INSTRUCTION_H
#define QUANTUMSIMULATOR_INSTRUCTION_H


#include <vector>
#include <random>
#include <cfloat>
#include "../math/Environment.h"

namespace compiler { namespace instructions {

	/**
	 * The main instruction class. The execute and print methods
	 * are automatically called by the containing program.
	 */
	class Instruction {
	public:

		/**
		 * An overridable destructor for the child classes.
		 */
		virtual ~Instruction();

		/**
		 * Prints the instruction to the given output stream,
		 * in the given mode.
		 *
		 * @param out The output stream where the instruction will be printed
		 * @param qe Enables quantum experience friendly mode
		 * @return The number of lines to be commented out after this line
		 */
		virtual unsigned long print(std::ostream &out, bool qe) = 0;

		/**
		 * Executes the instruction, that may or may not manipulate the given
		 * quantum environment.
		 *
		 * @param env The quantum environment
		 * @return The number of instructions to jump after this instruction
		 */
		virtual unsigned long execute(Environment &env) = 0;
	};

	/**
	 * Applies a unitary gate to a qubit.
	 *
	 * Gate transformation matrix:
	 * e^(-i(phi+lambda)/2)*cos(theta/2) -e^(-i(phi-lambda)/2)*sin(theta/2)
	 * e^(i(phi-lambda)/2)*sin(theta/2)   e^(i(phi+lambda)/2)*cos(theta/2)
	 */
	class U : public Instruction {
	private:

		double theta;
		double phi;
		double lambda;
		unsigned long qubit;

	public:

		U(double theta, double phi, double lambda, unsigned long qubit);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};

	/**
	 * Applies a controlled not gate to two qubits
	 *
	 * Gate transformation matrix:
	 * 1 0 0 0
	 * 0 0 0 1
	 * 0 0 1 0
	 * 0 1 0 0
	 */
	class CX : public Instruction {
	private:

		unsigned long qubit1;
		unsigned long qubit2;

	public:

		CX(unsigned long qubit1, unsigned long qubit2);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};

	/**
	 * Creates a barrier that prevents optimization of a qubit
	 * over this line. However in this simulation it does nothing.
	 */
	class Barrier : public Instruction {
	private:

		unsigned long qubit;

	public:

		explicit Barrier(unsigned long qubit);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};

	/**
	 * Resets a qubit to it's initial state and
	 * modifies all the quantum states accordingly.
	 */
	class Reset : public Instruction {
	private:

		unsigned long qubit;

	public:

		explicit Reset(unsigned long qubit);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};

	/**
	 * Sets a qubit's value randomly to either 0 or 1 according to it's probability,
	 * and sets it as the value of a real bit. After that it modifies
	 * all the quantum states accordingly.
	 */
	class Measure : public Instruction {
	private:

		static std::mt19937 mt;
		static std::uniform_real_distribution<double> distribution;

		unsigned long qubit;
		unsigned long bit;

		static double random();

	public:

		Measure(unsigned long qubit, unsigned long bit);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};

	/**
	 * Creates an integer from a list of real bits. If the integer
	 * doesn't match the criteria, the following "jump" amount of
	 * instructions are skipped.
	 */
	class Condition : public Instruction {
	private:

		std::vector<unsigned long> bits;
		unsigned long criteria;
		unsigned long jump;

	public:

		Condition(const std::vector<unsigned long> &bits, unsigned long criteria, unsigned long jump);

		unsigned long print(std::ostream &out, bool qe);
		unsigned long execute(Environment &env);
	};
} }

using namespace compiler;
using namespace instructions;


#endif //QUANTUMSIMULATOR_INSTRUCTION_H
