#ifndef QUANTUMSIMULATOR_COMPLEX_H
#define QUANTUMSIMULATOR_COMPLEX_H


#include <iostream>
#include <cmath>

namespace math {

	/**
	 * Represents a complex number by it's real and imaginary part.
	 * Both are real numbers (doubles).
	 */
	class Complex {
	public:

		double r;
		double i;

		/**
		 * Creates a complex number from it's real and imaginary part (both doubles).
		 *
		 * @param r The number's real part
		 * @param i The number's imaginary part
		 */
		Complex(double r = 0, double i = 0);

		/**
		 * Copies the input complex number's values and creates a new complex number.
		 *
		 * @param c The input complex number
		 */
		Complex(const Complex& c);

		/**
		 * Copies the input complex number's values
		 * and sets them as the current copmlex number's values.
		 *
		 * @param c The input token
		 * @return The complex number itself
		 */
		Complex& operator=(const Complex& c);

		/**
		 * Set's the current complex number's real part as the input real number and
		 * the imaginary part to 0.
		 *
		 * @param d The input real number
		 * @return The complex number itself
		 */
		Complex& operator=(double d);

		/**
		 * Return's the complex number's length squared.
		 *
		 * @return The length squared
		 */
		double lengthSquared() const;

		/**
		 * Return's the complex number's length.
		 *
		 * @return The length
		 */
		double length() const;

		/**
		 * Returns the complex number itself.
		 *
		 * @return The complex number itself.
		 */
		Complex operator+() const;

		/**
		 * Returns the complex number negated.
		 *
		 * @return The complex number negated.
		 */
		Complex operator-() const;
	};
}

using namespace math;

/**
	 * Adds two complex numbers together.
	 *
	 * @param a The first number
	 * @param b The second number
	 * @return The resulting complex number
	 */
Complex operator+(const Complex& a, const Complex& b);

/**
 * Adds a complex number to a real number.
 *
 * @param a The complex number
 * @param b The real number
 * @return The resulting complex number
 */
Complex operator+(const Complex& a, double b);

/**
 * Subtracts the first complex number from the second.
 *
 * @param a The first number
 * @param b The second number
 * @return The resulting complex number
 */
Complex operator-(const Complex& a, const Complex& b);

/**
 * Subtracts a real number from a complex number.
 *
 * @param a The complex number
 * @param b The real number
 * @return The resulting complex number
 */
Complex operator-(const Complex& a, double b);

/**
 * Multiplies two complex numbers together.
 *
 * @param a The first number
 * @param b The second number
 * @return The resulting complex number
 */
Complex operator*(const Complex& a, const Complex& b);

/**
 * Multiplies a complex number with a real number.
 *
 * @param a The complex number
 * @param b The real number
 * @return The resulting complex number
 */
Complex operator*(const Complex& a, double b);

/**
 * Divides the first complex number by the second.
 *
 * @param a The first number
 * @param b The second number
 * @return The resulting complex number
 */
Complex operator/(const Complex& a, const Complex& b);

/**
 * Divides a complex number by a real number
 *
 * @param a The complex number
 * @param b The real number
 * @return The resulting complex number
 */
Complex operator/(const Complex& a, double b);

/**
 * Put's the contents of the complex number to the given output stream.
 *
 * @param out The output stream
 * @param c The complex number
 * @return A reference the the given output stream
 */
std::ostream& operator<<(std::ostream& out, const Complex& c);


#endif //QUANTUMSIMULATOR_COMPLEX_H
