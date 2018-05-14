#include "Environment.h"

Environment::Environment(unsigned long bitCount, unsigned long qubitCount) : bitCount(bitCount), qubitCount(qubitCount) {
	bitValues = new unsigned int[bitCount];
	std::fill(bitValues, bitValues + bitCount, 0);

	stateCoefficients = new Complex[1ul << qubitCount];
	stateCoefficients[0] = 1;
}

Environment::~Environment() {
	delete[] bitValues;
	delete[] stateCoefficients;
}

unsigned long Environment::getBitCount() const {
	return bitCount;
}

unsigned long Environment::getQubitCount() const {
	return qubitCount;
}

unsigned long Environment::getStateCount() const {
	return 1ul << qubitCount;
}

unsigned int Environment::getBit(unsigned long bit) const {
	return bitValues[bit];
}

void Environment::setBit(unsigned long bit, unsigned int value) {
	if (value > 1) throw std::out_of_range("too big");
	bitValues[bit] = value;
}

Complex Environment::getStateCoefficient(unsigned long state) const {
	return stateCoefficients[state];
}

double Environment::getStateChance(unsigned long state) const {
	return stateCoefficients[state].lengthSquared();
}

double Environment::getQubitChance(unsigned long qubit) const {
	double chance = 0;
	unsigned long pos = 1ul << qubit;
	for (unsigned long state = 0; state < getStateCount() >> 1ul; state++) {
		if (state & pos) chance += stateCoefficients[state].lengthSquared();
	}
	return chance;
}

void Environment::applyTransform(unsigned long qubit, Complex matrix[2][2]) {
	unsigned long state = 0;
	unsigned long pos = 1ul << qubit;
	for (unsigned long i = 0; i < getStateCount() >> 1ul; i++, state++) {
		if (state & pos) state += pos;

		unsigned long state1 = state;
		unsigned long state2 = state + pos;

		Complex coefficient1 = stateCoefficients[state1];
		Complex coefficient2 = stateCoefficients[state2];

		stateCoefficients[state1] = coefficient1 * matrix[0][0] + coefficient2 * matrix[1][0];
		stateCoefficients[state2] = coefficient1 * matrix[0][1] + coefficient2 * matrix[1][1];
	}
}

void Environment::applyTransform(unsigned long qubit1, unsigned long qubit2, Complex matrix[4][4]) {
	unsigned long state = 0;
	unsigned long pos1 = 1ul << qubit1;
	unsigned long pos2 = 1ul << qubit2;
	for (unsigned long i = 0; i < getStateCount() >> 2ul; i++, state++) {
		if (qubit1 < qubit2) {
			if (state & pos1) state += pos1;
			if (state & pos2) state += pos2;
		} else {
			if (state & pos2) state += pos2;
			if (state & pos1) state += pos1;
		}

		unsigned long state1 = state;
		unsigned long state2 = state + pos1;
		unsigned long state3 = state + pos2;
		unsigned long state4 = state + pos1 + pos2;

		Complex coefficient1 = stateCoefficients[state1];
		Complex coefficient2 = stateCoefficients[state2];
		Complex coefficient3 = stateCoefficients[state3];
		Complex coefficient4 = stateCoefficients[state4];

		stateCoefficients[state1] = coefficient1 * matrix[0][0] + coefficient2 * matrix[1][0] + coefficient3 * matrix[2][0] + coefficient4 * matrix[3][0];
		stateCoefficients[state2] = coefficient1 * matrix[0][1] + coefficient2 * matrix[1][1] + coefficient3 * matrix[2][1] + coefficient4 * matrix[3][1];
		stateCoefficients[state3] = coefficient1 * matrix[0][2] + coefficient2 * matrix[1][2] + coefficient3 * matrix[2][2] + coefficient4 * matrix[3][2];
		stateCoefficients[state4] = coefficient1 * matrix[0][3] + coefficient2 * matrix[1][3] + coefficient3 * matrix[2][3] + coefficient4 * matrix[3][3];
	}
}

void Environment::normalize() {
	double sum = 0;
	for (unsigned long state = 0; state < getStateCount(); state++) sum += getStateChance(state);
	double scale = 1.0 / sqrt(sum);
	for (unsigned long state = 0; state < getStateCount(); state++) {
		stateCoefficients[state] = stateCoefficients[state] * scale;
	}
}
