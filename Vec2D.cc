#include "Vec2D.h"


// Default constructor
Vec2D::Vec2D()
{
    setZero();
}


// Alternative constructor, lets you define all three elements.
Vec2D::Vec2D(const double x, const double y)
{
    X = x;
    Y = y;
}

// Sets each element to zero.
void Vec2D::setZero()
{
    X = 0.0;
    Y = 0.0;
}


// Adds vector to itself
Vec2D Vec2D::operator +(const Vec2D& a) const
        {
    return Vec2D(X + a.X, Y + a.Y);
}

// Subtracts vector from itself
Vec2D Vec2D::operator -(const Vec2D& a) const
        {
    return Vec2D(X - a.X, Y - a.Y);
}

// Multiplies each element with a scalar
Vec2D Vec2D::operator *(const double a) const
{
    return Vec2D(X * a, Y * a);
}

// Divides each element by a scalar
Vec2D Vec2D::operator /(const double a) const
        {
    return Vec2D(X / a, Y / a);
}


// Adds a vector to itself
Vec2D& Vec2D::operator+=(const Vec2D& a)
{
    X += a.X;
    Y += a.Y;
    return *this;
}

// Subtracts a vector from itself
Vec2D& Vec2D::operator-=(const Vec2D& a)
{
    X -= a.X;
    Y -= a.Y;
    return *this;
}

// Multiplies each element by a scalar
Vec2D& Vec2D::operator*=(const double a)
{
    X *= a;
    Y *= a;
    return *this;
}

// Divides each element by a scalar
Vec2D& Vec2D::operator/=(const double a)
{
    X /= a;
    Y /= a;
    return *this;
}

// Divides each element by a scalar and rounds off
double Vec2D::roundOff_x(const Vec2D& b, double a)
        {
    return round(b.X / a);
}

// Divides each element by a scalar and rounds off
double Vec2D::roundOff_y(const Vec2D& b, double a)
        {
    return round(b.Y / a);
}

/*!
 * \details Calculates the dot product of two vectors.
 * NB: this is a STATIC function!
*/
double Vec2D::dot(const Vec2D& a, const Vec2D& b)
{
    return a.X * b.X + a.Y * b.Y;
}

/*!
 * \details Calculates the square of the length of itself
 */
double Vec2D::getLengthSquared() const
{
    return (X * X + Y * Y);
}

/*!
 * \details returns the vector element belonging to the given index.
 * \param[in] index     the index of interest (should be 0, 1 or 2)
 */
double Vec2D::getComponent(const int index) const
        {
    switch (index)
    {
        case 0:
            return X;
        case 1:
            return Y;
        default:
            std::cerr << "Enter the correct index." << std::endl;
            return 0;
    }
}

/*!
 * Sets the element of the vector belonging to the first argument 
 * (index) to the value given in the second argument (val).
*/
void Vec2D::setComponent(const int index, const double val)
{
    switch (index)
    {
        case 0:
            X = val;
            break;
        case 1:
            Y = val;
            break;
        default:
            std::cerr << "Set the correct component" << std::endl;
    }
}

/*!
 * Calculates the length of this vector
 */
double Vec2D::getLength() const
{
    return std::sqrt(getLengthSquared());
}


/*
 * Adds all elements of the vector to an output stream.
 * NB: this is a global function and a friend of the Vec2D class!
 */
std::ostream& operator<<(std::ostream& os, const Vec2D& a)
{
    os << a.X << ' ' << a.Y;
    return os;
}

/*
 * Reads all elements of a given vector from an input stream.
 * NB: this is a global function and a friend of the Vec2D class!
 */
std::istream& operator>>(std::istream& is, Vec2D& a)
{
    is >> a.X >> a.Y;
    return is;
}

/*
 * Returns the negative of a given vector.
 * NB: this is a global function and a friend of the Vec2D class. Gets called when
 * a negation operation of the form - (Vec2D) is performed. 
*/
Vec2D operator-(const Vec2D& a)
{
    return Vec2D(-a.X, -a.Y);
}

/*
 * Multiplies each element of a given vector (b) by a given scalar (a).
 * NB: this is a global function and a friend of the Vec2D class. Gets called when
 * a scalar multiplication of the form (Mdouble) * (Vec2D) is performed.
 */
Vec2D operator*(const double a, const Vec2D& b)
{
    return Vec2D(b.X * a, b.Y * a);
}
