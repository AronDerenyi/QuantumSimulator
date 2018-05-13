#include "Complex.h"

Complex::Complex(double r, double i) : r(r), i(i) {}

Complex::Complex(const Complex& c) : r(c.r), i(c.i) {}

Complex& Complex::operator=(const Complex& c) {
	r = c.r;
	i = c.i;
	return *this;
}

Complex& Complex::operator=(double d) {
	r = d;
	i = 0;
	return *this;
}

double Complex::lengthSquared() const {
	return r * r + i * i;
}

double Complex::length() const {
	return sqrt(lengthSquared());
}

Complex Complex::operator+() const {
	return *this;
}

Complex Complex::operator-() const {
	return *this * -1;
}

Complex operator+(const Complex& a, const Complex& b) {
	return Complex(a.r + b.r, a.i + b.i);
}

Complex operator+(const Complex& a, double b) {
	return Complex(a.r + b, a.i);
}

Complex operator-(const Complex& a, const Complex& b) {
	return Complex(a.r - b.r, a.i - b.i);
}

Complex operator-(const Complex& a, double b) {
	return Complex(a.r - b, a.i);
}

Complex operator*(const Complex& a, const Complex& b) {
	return Complex(a.r * b.r - a.i * b.i, a.r * b.i + a.i * b.r);
}

Complex operator*(const Complex& a, double b) {
	return Complex(a.r * b, a.i * b);
}

Complex operator/(const Complex& a, const Complex& b) {
	double denominator = b.r * b.r + b.i * b.i;
	return Complex((a.r * b.r + a.i * b.i) / denominator, (a.i * b.r - a.r * b.i) / denominator);
}

Complex operator/(const Complex& a, double b) {
	return Complex(a.r / b, a.i / b);
}

std::ostream& operator<<(std::ostream& out, const Complex& c) {
	return out << "(" << c.r << " + " << c.i << "i)";
}
