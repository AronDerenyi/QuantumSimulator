#ifndef QUANTUMSIMULATOR_ENVIRONMENT_H
#define QUANTUMSIMULATOR_ENVIRONMENT_H


#include "Complex.h"

namespace math {

	/**
	 * The quantum environment controls the states in the environment.
	 */
	class Environment {
	private:

		unsigned long bitCount;
		unsigned int *bitValues;

		unsigned long qubitCount;
		Complex *stateCoefficients;

	public:

		/**
		 * Creates a new environment with the given bit and qubit count.
		 *
		 * @param bitCount The number of real bits in the environment
		 * @param qubitCount The number of quantim bits in the environment
		 */
		Environment(unsigned long bitCount, unsigned long qubitCount);

		/**
		 * Deletes the real bits and quantum bits (their array).
		 */
		~Environment();

		/**
		 * Returns the number of real bits in the environment.
		 *
		 * @return The bit count
		 */
		unsigned long getBitCount() const;

		/**
		 * Returns the number of quantum bits in the environment.
		 *
		 * @return The qubit count
		 */
		unsigned long getQubitCount() const;

		/**
		 * Returns the number of possible states of the environment.
		 * (2 ^ qubit count)
		 *
		 * @return The number of the environment's states
		 */
		unsigned long getStateCount() const;

		/**
		 * Returns the value of a real bit.
		 *
		 * @param bit The id of the real bit
		 * @return The value of a real bit
		 */
		unsigned int getBit(unsigned long bit) const;

		/**
		 * Set's the value of a real bit.
		 *
		 * @param bit The id of the real bit
		 * @param value The value of the real bit
		 */
		void setBit(unsigned long bit, unsigned int value);

		/**
		 * Returns a complex number describing the probability of a state.
		 *
		 * @param state The id of the state
		 * @return The state's probability
		 */
		Complex getStateCoefficient(unsigned long state) const;

		/**
		 * Returns a real number describing the probability of a state.
		 *
		 * @param state The id of the state
		 * @return The state's probability
		 */
		double getStateChance(unsigned long state) const;

		/**
		 * Returns the probability of the given qubit bein 1.
		 *
		 * @param qubit The id of the qubit
		 * @return The probability of the qubit being 1
		 */
		double getQubitChance(unsigned long qubit) const;

		/**
		 * Applies a 2x2 matrix transformation to a qubit in the environment.
		 *
		 * @param qubit The id of the qubit
		 * @param matrix The transformation (2x2 complex matrix)
		 */
		void applyTransform(unsigned long qubit, Complex matrix[2][2]);

		/**
		 * Applies a 4x4 matrix transformation to two qubits in the environment.
		 *
		 * @param qubit1 The id of the first qubit
		 * @param qubit2 The id of the second qubit
		 * @param matrix The transformation (4x4 complex matrix)
		 */
		void applyTransform(unsigned long qubit1, unsigned long qubit2, Complex matrix[4][4]);

		/**
		 * Normalizes the environment, so that the total sum of probabilities is 1.
		 */
		void normalize();
	};
}

using namespace math;


#endif //QUANTUMSIMULATOR_ENVIRONMENT_H
