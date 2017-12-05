#include "Vec3D.h"


// Default constructor
Vec3D::Vec3D()
{
    setZero();
}


// Alternative constructor, lets you define all three elements.
Vec3D::Vec3D(const double x, const double y, const double z)
{
    X = x;
    Y = y;
    Z = z;
}

// Sets each element to zero.
void Vec3D::setZero()
{
    X = 0.0;
    Y = 0.0;
    Z = 0.0;
}

// Adds vector to itself
Vec3D Vec3D::operator +(const Vec3D& a) const
        {
    return Vec3D(X + a.X, Y + a.Y, Z + a.Z);
}

// Subtracts vector from itself
Vec3D Vec3D::operator -(const Vec3D& a) const
        {
    return Vec3D(X - a.X, Y - a.Y, Z - a.Z);
}

// Multiplies each element with a scalar
Vec3D Vec3D::operator *(const double a) const
{
    return Vec3D(X * a, Y * a, Z * a);
}

// Divides each element by a scalar
Vec3D Vec3D::operator /(const double a) const
        {
    return Vec3D(X / a, Y / a, Z / a);
}


 // Adds a vector to itself
Vec3D& Vec3D::operator+=(const Vec3D& a)
{
    X += a.X;
    Y += a.Y;
    Z += a.Z;
    return *this;
}

// Subtracts a vector from itself
Vec3D& Vec3D::operator-=(const Vec3D& a)
{
    X -= a.X;
    Y -= a.Y;
    Z -= a.Z;
    return *this;
}

// Multiplies each element by a scalar
Vec3D& Vec3D::operator*=(const double a)
{
    X *= a;
    Y *= a;
    Z *= a;
    return *this;
}

// Divides each element by a scalar
Vec3D& Vec3D::operator/=(const double a)
{
    X /= a;
    Y /= a;
    Z /= a;
    return *this;
}

// Divides each element by a scalar and rounds off
double Vec3D::roundOff_x(const Vec3D& b, double a)
        {
    return round(b.X / a);
}

// Divides each element by a scalar and rounds off
double Vec3D::roundOff_y(const Vec3D& b, double a)
        {
    return round(b.Y / a);
}

// Divides each element by a scalar and rounds off
double Vec3D::roundOff_z(const Vec3D& b, double a)
        {
    return round(b.Z / a);
}
/*!
 * \details Calculates the dot product of two vectors.
 * NB: this is a STATIC function!
*/
double Vec3D::dot(const Vec3D& a, const Vec3D& b)
{
    return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

/*!
 * \details Calculates the square of the length of itself
 */
double Vec3D::getLengthSquared() const
{
    return (X * X + Y * Y + Z * Z);
}

/*!
 * \details returns the vector element belonging to the given index.
 * \param[in] index     the index of interest (should be 0, 1 or 2)
 */
double Vec3D::getComponent(const int index) const
        {
    switch (index)
    {
        case 0:
            return X;
        case 1:
            return Y;
        case 2:
            return Z;
        default:
            std::cerr << "Enter the correct index." << std::endl;
            return 0;
    }
}

/*!
 * Sets the element of the vector belonging to the first argument 
 * (index) to the value given in the second argument (val).
*/
void Vec3D::setComponent(const int index, const double val)
{
    switch (index)
    {
        case 0:
            X = val;
            break;
        case 1:
            Y = val;
            break;
        case 2:
            Z = val;
            break;
        default:
            std::cerr << "Set the correct component" << std::endl;
    }
}

/*!
 * Calculates the length of this vector
 */
double Vec3D::getLength() const
{
    return std::sqrt(getLengthSquared());
}


/*
 * Adds all elements of the vector to an output stream.
 * NB: this is a global function and a friend of the Vec3D class!
 */
std::ostream& operator<<(std::ostream& os, const Vec3D& a)
{
    os << a.X << ' ' << a.Y << ' ' << a.Z;
    return os;
}

/*
 * Reads all elements of a given vector from an input stream.
 * NB: this is a global function and a friend of the Vec3D class!
 */
std::istream& operator>>(std::istream& is, Vec3D& a)
{
    is >> a.X >> a.Y >> a.Z;
    return is;
}

/*
 * Returns the negative of a given vector.
 * NB: this is a global function and a friend of the Vec3D class. Gets called when
 * a negation operation of the form - (Vec3D) is performed. 
*/
Vec3D operator-(const Vec3D& a)
{
    return Vec3D(-a.X, -a.Y, -a.Z);
}

/*
 * Multiplies each element of a given vector (b) by a given scalar (a).
 * NB: this is a global function and a friend of the Vec3D class. Gets called when
 * a scalar multiplication of the form (Mdouble) * (Vec3D) is performed.
 */
Vec3D operator*(const double a, const Vec3D& b)
{
    return Vec3D(b.X * a, b.Y * a, b.Z * a);
}
