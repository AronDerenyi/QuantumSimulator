#include "Instruction.h"

Instruction::~Instruction() {

}

// U

U::U(double theta, double phi, double lambda, unsigned long qubit) :
		theta(theta), phi(phi), lambda(lambda), qubit(qubit) {

}

unsigned long U::print(std::ostream &out, bool qe) {
	out << "u";
	if (qe) out << "3";

	out << " (" << theta << ", " << phi << ", " << lambda << ") ";
	out << "q[" << qubit << "]";
	out << ";" << std::endl;
	return 0;
}

unsigned long U::execute(Environment &env) {
	double c = cos(theta / 2);
	double s = sin(theta / 2);

	double exp[2][2] {
			{-(phi + lambda) / 2, (phi - lambda) / 2},
			{-(phi - lambda) / 2, (phi + lambda) / 2}
	};

	Complex matrix[2][2] {
			{Complex(cos(exp[0][0]) * c, sin(exp[0][0]) * c), Complex(cos(exp[0][1]) * s, sin(exp[0][1]) * s)},
			{Complex(-cos(exp[1][0]) * s, -sin(exp[1][0]) * s), Complex(cos(exp[1][1]) * c, sin(exp[1][1]) * c)}
	};

	env.applyTransform(qubit, matrix);
	return 0;
}

// CX

CX::CX(unsigned long qubit1, unsigned long qubit2) :
		qubit1(qubit1), qubit2(qubit2) {

}

unsigned long CX::print(std::ostream &out, bool qe) {
	out << "cx";

	out << " ";
	out << "q[" << qubit1 << "]";
	out << ", ";
	out << "q[" << qubit2 << "]";
	out << ";" << std::endl;
	return 0;
}

unsigned long CX::execute(Environment &env) {
	Complex matrix[4][4] = {
			{1, 0, 0, 0},
			{0, 0, 0, 1},
			{0, 0, 1, 0},
			{0, 1, 0, 0}
	};

	env.applyTransform(qubit1, qubit2, matrix);
	return 0;
}

// BARRIER

Barrier::Barrier(unsigned long qubit) :
		qubit(qubit) {

}

unsigned long Barrier::print(std::ostream &out, bool qe) {
	std::cout << "barrier";

	std::cout << " ";
	std::cout << "q[" << qubit << "]";
	std::cout << ";" << std::endl;
	return 0;
}

unsigned long Barrier::execute(Environment &env) {
	return 0;
}

// RESET

Reset::Reset(unsigned long qubit) :
		qubit(qubit) {

}

unsigned long Reset::print(std::ostream &out, bool qe) {
	std::cout << "reset";

	std::cout << " ";
	std::cout << "q[" << qubit << "]";
	std::cout << ";" << std::endl;
	return 0;
}

unsigned long Reset::execute(Environment &env) {
	Complex matrix[2][2] {
			{1, 0},
			{0, 0}
	};

	env.applyTransform(qubit, matrix);
	env.normalize();
	return 0;
}

// MEASURE

std::mt19937 Measure::mt = std::mt19937(std::random_device()());
std::uniform_real_distribution<double> Measure::distribution = std::uniform_real_distribution<double>(0, std::nextafter(1, DBL_MAX));

double Measure::random() {
	return distribution(mt);
}

Measure::Measure(unsigned long qubit, unsigned long bit) :
		qubit(qubit), bit(bit) {

}

unsigned long Measure::print(std::ostream &out, bool qe) {
	std::cout << "measure";

	std::cout << " ";
	std::cout << "q[" << qubit << "]";
	std::cout << " -> ";
	std::cout << "c[" << bit << "]";
	std::cout << ";" << std::endl;
	return 0;
}

unsigned long Measure::execute(Environment &env) {
	double random = Measure::random();
	double chance = env.getQubitChance(qubit);
	unsigned int result = random > chance ? 0 : 1;

	Complex matrix[2][2] {
			{1 - result, 0},
			{0, result}
	};

	env.applyTransform(qubit, matrix);
	env.normalize();
	env.setBit(bit, result);
	return 0;
}

// CONDITION

Condition::Condition(const std::vector<unsigned long> &bits, unsigned long criteria, unsigned long jump) :
		bits(bits), criteria(criteria), jump(jump) {

}

unsigned long Condition::print(std::ostream &out, bool qe) {
	if (qe) std::cout << "// conditions are not supported in the Quantum Experience ";
	std::cout << "condition";
	std::cout << " (";
	for (unsigned long bit : bits) {
		if (bit != bits[0]) std::cout << ", ";
		std::cout << "c[" << bit << "]";
	}
	std::cout << " == ";
	std::cout << criteria;
	std::cout << "): ";
	std::cout << jump;
	std::cout << ";" << std::endl;
	return qe ? jump : 0;
}

unsigned long Condition::execute(Environment &env) {
	unsigned long value = 0;
	for (unsigned long i = 0; i < bits.size(); i++) {
		value += (env.getBit(bits[i]) << i);
	}

	if (value != criteria) {
		return jump;
	} else {
		return 0;
	}
}
