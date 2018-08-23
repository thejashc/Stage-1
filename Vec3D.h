#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <sstream>
#include <iostream>
#include <stdlib.h> 

/*!
 * \class Vec3D
 * \brief
 */
class Vec3D
{
public:
    
    // the vector components
    double X, Y, Z;

    // constructor
    Vec3D();
    
    // Alternative constructor, taking the three elements as arguments
    Vec3D(const double x, const double y, const double z);
    
    // Sets all elements to zero
    void setZero();
        
    // Adds another vector
    Vec3D operator +(const Vec3D& a) const;
    
    // Subtracts another vector
    Vec3D operator -(const Vec3D& a) const;
        
    // Multiplies by a scalar
    Vec3D operator *(const double a) const;
    
    // Divides by a scalar
    Vec3D operator /(const double a) const;
    
    // Adds a vector to itself
    Vec3D& operator+=(const Vec3D& a);
    
    // Subtracts a vector from itself
    Vec3D& operator-=(const Vec3D& a);
    
    // Multiplies itself by a scalar 
    Vec3D& operator*=(const double a);
    
    // Divides itself by a scalar
    Vec3D& operator/=(const double a);

    // Cross Product between two vectors
    // Vec3D cross( Vec3D a, Vec3D b);
    Vec3D operator %(const Vec3D& b) const;
    
    // Divides itself by a scalar and rounds off the value
    static double roundOff_x(const Vec3D& b, double a);
    static double roundOff_y(const Vec3D& b, double a);
    static double roundOff_z(const Vec3D& b, double a);

    // Calculates the dot product of two Vec3D: \f$ a \cdot b\f$
    static double dot(const Vec3D& a, const Vec3D& b);

    // Calculates the length of this Vec3D: \f$ \sqrt{a\cdot a} \f$
    double getLength() const;

    // Calculates the squared length of this Vec3D: \f$ a\cdot a \f$
    double getLengthSquared() const;

    // Returns the requested component of this Vec3D
    double getComponent(const int index) const;

    // Sets the requested component of this Vec3D to the requested value    
    void setComponent(const int index, const double val);

    // Adds elements to an output stream    
    friend std::ostream& operator<<(std::ostream& os, const Vec3D& a);

    // Adds elements to an input stream
    friend std::istream& operator>>(std::istream& is, Vec3D& a);

    // Subtracts a vector 
    friend Vec3D operator-(const Vec3D& a);

    // Multiplies all elements by a scalar    
    friend Vec3D operator*(const double a, const Vec3D& b);    
};

#endif
