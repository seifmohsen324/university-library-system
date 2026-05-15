#ifndef PROJECT_LOAN_H
#define PROJECT_LOAN_H

// Include necessary libraries and files
#include "Custom_String_Class.h"
#include "Date.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "json.h"

// Use nlohmann's json library
using json = nlohmann::json;

// Loan class definition
class Loan  {
private:
    // Private member variables
    int memberID; // Member ID
    Custom_String_Class bookID; // Book ID
    Date dueDate; // Due date
    Date borrowDate; // Borrow date
    friend class Librarian; // Friend class declaration

public:
    // Public member functions

    // Constructors
    Loan(); // Default constructor
    Loan(int, const Custom_String_Class&, Date, Date); // Constructor with member ID, book ID, due date, and borrow date
    Loan(int, const Custom_String_Class&, Date); // Constructor with member ID, book ID, and due date. Borrow date is set to current date.
    Loan(int m, const Custom_String_Class& b); // Constructor with member ID and book ID. Due date and borrow date are set to 0.

    // Getters
    Custom_String_Class getBookID() const; // Getter for book ID
    int getMemberID() const; // Getter for member ID
    Date getDueDate() const; // Getter for due date
    Date getBorrowDate() const; // Getter for borrow date
    static vector<Loan>& getLoans_List(); // Getter for list of loans

    // Setter
    void setBorrowDate(Date); // Setter for borrow date

    // Other member functions
    static bool loadLoans(); // Function to load loans from a file
    static void saveLoans(); // Function to save loans to a file
    int calculateFines(); // Function to calculate fines
    static void displaylist(); // Function to display the list of loans
};

#endif //PROJECT_LOAN_H
