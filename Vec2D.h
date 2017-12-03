#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <sstream>
#include <iostream>
#include <stdlib.h> 

/*!
 * \class Vec2D
 * \brief
 */
class Vec2D
{
	public:

		// the vector components
		double X, Y;

		// constructor
		Vec2D();

		// Alternative constructor, taking the three elements as arguments
		Vec2D(const double x, const double y);

		// Sets all elements to zero
		void setZero();
		
		// Adds another vector
		Vec2D operator +(const Vec2D& a) const;

		// Subtracts another vector
		Vec2D operator -(const Vec2D& a) const;

		// Multiplies by a scalar
		Vec2D operator *(const double a) const;

		// Divides by a scalar
		Vec2D operator /(const double a) const;

		// Adds a vector to itself
		Vec2D& operator+=(const Vec2D& a);

		// Subtracts a vector from itself
		Vec2D& operator-=(const Vec2D& a);

		// Multiplies itself by a scalar 
		Vec2D& operator*=(const double a);

		// Divides itself by a scalar
		Vec2D& operator/=(const double a);

		// Divides itself by a scalar and rounds off the value
		static double roundOff_x(const Vec2D& b, double a);
		static double roundOff_y(const Vec2D& b, double a);
		
		// Calculates the dot product of two Vec2D: \f$ a \cdot b\f$
		static double dot(const Vec2D& a, const Vec2D& b);

		// Calculates the length of this Vec2D: \f$ \sqrt{a\cdot a} \f$
		double getLength() const;

		// Calculates the squared length of this Vec2D: \f$ a\cdot a \f$
		double getLengthSquared() const;

		// Returns the requested component of this Vec2D
		double getComponent(const int index) const;

		// Sets the requested component of this Vec2D to the requested value    
		void setComponent(const int index, const double val);

		// Adds elements to an output stream    
		friend std::ostream& operator<<(std::ostream& os, const Vec2D& a);

		// Adds elements to an input stream
		friend std::istream& operator>>(std::istream& is, Vec2D& a);

		// Subtracts a vector 
		friend Vec2D operator-(const Vec2D& a);

		// Multiplies all elements by a scalar    
		friend Vec2D operator*(const double a, const Vec2D& b);    
};

#endif
