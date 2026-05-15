#include "Date.h"
#include <sstream>
#include <iomanip>

namespace {
bool parseDateString(const Custom_String_Class& input, int& day, int& month, int& year) {
    std::stringstream ss(input.toString());
    char firstSlash = '\0';
    char secondSlash = '\0';

    if (!(ss >> day >> firstSlash >> month >> secondSlash >> year)) {
        return false;
    }

    ss >> std::ws;
    return firstSlash == '/' && secondSlash == '/' && ss.eof();
}
}

// Default constructor initializing date to 0/0/0
Date::Date():day(0), month(0), year(0){}

// Parameterized constructor to create a date with given day, month, and year
Date::Date(int d, int m, int y): day(d), month(m), year(y){}

// Constructor that takes a string in the format "dd/mm/yyyy"
Date::Date(Custom_String_Class d) {
    // Accept dates like dd/mm/yyyy and legacy 00/00/0 sentinel values.
    if (!parseDateString(d, day, month, year)) {
        cerr << "Error: Invalid date format. Expected dd/mm/yyyy" << endl;
        day = 0; month = 0; year = 0;
        return;
    }

    if (day == 0 && month == 0 && year == 0) {
        return;
    }

    // Validate date range
    if (month < 1 || month > 12 || year < 0 ||
        day < 1 || day > daysInMonth(month, year)) {
        cerr << "Error: Invalid date values for the selected month/year" << endl;
        day = 0; month = 0; year = 0;
    }
}

// Function to set the date from a string in the format "dd/mm/yyyy"
void Date::setDate (Custom_String_Class d){
    // Accept dates like dd/mm/yyyy and legacy 00/00/0 sentinel values.
    if (!parseDateString(d, day, month, year)) {
        cerr << "Error: Invalid date format. Expected dd/mm/yyyy" << endl;
        day = 0; month = 0; year = 0;
        return;
    }

    if (day == 0 && month == 0 && year == 0) {
        return;
    }

    // Validate date range
    if (month < 1 || month > 12 || year < 0 ||
        day < 1 || day > daysInMonth(month, year)) {
        cerr << "Error: Invalid date values for the selected month/year" << endl;
        day = 0; month = 0; year = 0;
    }
}

// Function to get the date as a string in the format "dd/mm/yyyy"
Custom_String_Class Date::getDate() const{
    std::ostringstream out;
    out << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << std::setw(4) << year;
    return Custom_String_Class(out.str());
}

// Function to get the current date
Date Date::getCrrentDate(){
    // Get the current system time point
    auto now = chrono::system_clock::now();

    time_t currentTime = chrono::system_clock::to_time_t(now);

    tm* localTime = localtime(&currentTime);

    int year = localTime->tm_year + 1900;
    int month = localTime->tm_mon + 1;
    int day = localTime->tm_mday;

    return {day,month,year};
}

// Overloaded greater than operator to compare two dates
bool Date::operator>( Date d)  {
    int days =*this-d;
    return days>0;  // Return true if day is greater, false otherwise
}

// Overloaded less than operator to compare two dates
bool Date::operator<( Date d) {
    int days =*this-d;
    return days<0; // Return true if day is less, false otherwise
}

// Overloaded equality operator to compare two dates
bool Date::operator == (Date d) const{
    if (year==d.year && month==d.month && day==d.day)
        return true;
    else
        return false;
}

// Overloaded assignment operator to assign one date to another
Date& Date::operator = (Date d){
    day=d.day;
    month=d.month;
    year=d.year;
    return *this;
}

// Overloaded assignment operator to assign a string to a date
Date& Date::operator = (const Custom_String_Class& d){
    setDate(d);

    return *this;
}

// Overloaded addition operator to add days to a date
Date Date::operator + (int days) const{
    Date result(*this);
    result.day += days;
    while (true) {
        int daysinmonth = daysInMonth(result.month, result.year);
        if (result.day <= daysinmonth) {
            break;
        }
        result.day -= daysinmonth;
        if(result.month==12) {
            result.month = 1;
            result.year++;
        }
        else
            result.month++;
    }
    return result;
}

// Overloaded subtraction operator to find the difference between two dates
int Date::operator - (const Date& d) const{
    int thisDays = daysSinceReferenceDate();
    int dDays = d.daysSinceReferenceDate();

    return thisDays - dDays;
}

// Function to calculate the number of days since a reference date
int Date::daysSinceReferenceDate() const {
    int days = 0;
    for (int yr = REFERENCE_YEAR; yr < year; ++yr) {
        days += ((yr % 4 == 0 && yr % 100 != 0) || (yr % 400 == 0)) ? 366 : 365;
    }
    for (int mon = 1; mon < month; ++mon) {
        days += daysInMonth(mon, year);
    }
    days += day;
    return days;
}

// Function to calculate the number of days in a month
int Date::daysInMonth(int m, int y) {
    switch (m) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))
                return 29;
            else
                return 28;
        default:
            return -1; // Invalid month
    }
}
