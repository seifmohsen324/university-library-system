#ifndef PROJECT_DATE_H
#define PROJECT_DATE_H

#include "Custom_String_Class.h"
#include <iostream>
#include <chrono>
#include <ctime>

// Define a reference year for date calculations
#define REFERENCE_YEAR 1970

// Date class declaration
class Date {
    int day;   // Day of the date
    int month; // Month of the date
    int year;  // Year of the date

public:
    // Default constructor initializes date to 0/0/0
    Date();

    // Copy constructor
    Date(const Date&) = default;

    // Constructor that initializes date with day, month, and year
    Date(int, int, int);

    // Constructor that initializes date from a Custom_String_Class object
    Date(Custom_String_Class);

    // Function to set the date from a Custom_String_Class object
    void setDate (Custom_String_Class);

    // Function to get the date as a Custom_String_Class object
    Custom_String_Class getDate() const;

    // Static function to get the current date
    static Date getCrrentDate();

    // Comparison operators to compare two dates
    bool operator > (Date);
    bool operator < (Date);
    bool operator == (Date d) const;

    // Assignment operators to set the date
    Date& operator = (Date);
    Date& operator = (const Custom_String_Class&);

    // Addition operator to add days to the date
    Date operator + (int) const;

    // Subtraction operator to find the difference in days between two dates
    int operator - (const Date& d) const;

    // Function to calculate the number of days since a reference date
    int daysSinceReferenceDate() const;

    // Static function to calculate the number of days in a given month and year
    static int daysInMonth(int, int);
};

#endif //PROJECT_DATE_H
