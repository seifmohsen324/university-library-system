#ifndef LIBRARY_ENTITY_H
#define LIBRARY_ENTITY_H

#include "Custom_String_Class.h"
#include <iostream>
using namespace std;


// LibraryEntity — Abstract Base Class

class LibraryEntity {
public:
    // Pure virtual functions
    virtual void display() = 0;
    virtual Custom_String_Class getName() const = 0;
    virtual int getID() const = 0;
    virtual Custom_String_Class getRole() const = 0;
    virtual int getBorrowLimit() const = 0;

    // Virtual destructor
    virtual ~LibraryEntity() {}
};

#endif // LIBRARY_ENTITY_H

