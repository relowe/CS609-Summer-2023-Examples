/*
 * File: distance.cpp
 * Purpose: Allow the user to enter two 2d cartesian points and display 
 *          the distance and midpoint between them.
 */
#include <iostream>   // <-- include the iostream file here.
                      // Things beginning with # are preprocessor directives.
#include <cctype>
#include <cmath>

// Class Definition
//   - Member Variables
//   - Prototypes for the methods
class Point 
{
public:
    // constructors
    Point();
    Point(double _x, double _y);

    // accessors and mutators
    double x() const;
    double y() const;
    void x(double _x);
    void y(double _y);

    // subtraction operator for the point
    Point operator-(const Point &rhs);

private:
    double _x;
    double _y;
};


// Overloaded insertion and extraction operator
std::ostream& operator<<(std::ostream &os, const Point &p);
std::istream& operator>>(std::istream &is, Point &p);

//helper functions
void skip_non_digit(std::istream &is);


// The entry point of a C++ prograrm is the main function.
// Main function can optionally include command line parameters:
//    - int main()
//    - int main(int argc, char **argv)
// This function returns 0 on success, and any other number on failure.
int main()
{
    Point p1, p2;  // <- This creates two points!
    Point delta;
    double dist;

    // greet the user
    //   Insertion operator << 
    std::cout << "Point Distance Calculator" << std::endl;
    // ^ (::) Scope Resolution Modifier
    
    // Extraction operator >> does input
    std::cout << "P1 (x,y): ";
    std::cin >> p1;
    std::cout << "P2 (x,y): ";
    std::cin >> p2;

    // compute the diference between p2 and p1
    delta= p2 - p1;
    std::cout << "Distance=" 
              << sqrt(pow(delta.x(), 2) + pow(delta.y(), 2))
              << std::endl;


    // print the midpoint
    Point mid{ (p1.x()+p2.x())/2.0, (p1.y()+p2.y())/2.0 };
    std::cout << "Midpoint: " << mid << std::endl;

    return 0;   // A succesful return
}







///////////////////////////////////////////////////////////
// Point Impelmentation
///////////////////////////////////////////////////////////

// constructors
Point::Point() // Point::<name> indicates that <name> is part of Point
{
    x(0);
    y(0);
}


Point::Point(double _x, double _y)
{
    x(_x);
    y(_y);
}


// accessors and mutators
double Point::x() const
{
    return _x;
}


double Point::y() const
{
    return _y;
}


void Point::x(double _x)
{
    this->_x = _x;
}


void Point::y(double _y)
{
    this->_y = _y;
}


// subtraction operator for the point
Point Point::operator-(const Point &rhs)
{
    Point p;

    p.x(x() - rhs.x());
    p.y(y() - rhs.y());

    return p;
}


// insertion operator
std::ostream& operator<<(std::ostream &os, const Point &p)
{
    os << "(" << p.x() << ", " << p.y() << ")";

    return os;
}


// a helper function
void skip_non_digit(std::istream &is) 
{
    // skip all non-digits
    while(not isdigit(is.peek()))  is.get();
}


// extraction operator
std::istream& operator>>(std::istream &is, Point &p)
{
    double x,y;

    skip_non_digit(is);
    is >> x;

    skip_non_digit(is);
    is >> y;

    p.x(x);
    p.y(y);

    return is;
}
