#include "Loan.h"
#include "LibrarySystem.h"
#include "DataPaths.h"
#include <iomanip>

// Default constructor
Loan::Loan(): memberID(0),bookID(""), dueDate(0,0,0), borrowDate(0,0,0){}

// Constructor with member ID, book ID, due date, and borrow date
Loan::Loan(int m, const Custom_String_Class& b, Date due, Date borrow): memberID(m), bookID(b), dueDate(due), borrowDate(borrow) {}

// Constructor with member ID, book ID, and due date. Borrow date is set to current date.
Loan::Loan(int m, const Custom_String_Class& b, Date d): memberID(m), bookID(b), dueDate(d) {
    borrowDate=Date::getCrrentDate();
}

// Constructor with member ID and book ID. Due date and borrow date are set to 0.
Loan::Loan(int m, const Custom_String_Class& b): memberID(m), bookID(b), dueDate(0,0,0), borrowDate(0,0,0) {}

// Getter for book ID
Custom_String_Class Loan::getBookID() const{
    return bookID;
}

// Getter for member ID
int Loan::getMemberID() const{
    return memberID;
}

// Getter for due date
Date Loan::getDueDate() const{
    return dueDate;
}

// Getter for borrow date
Date Loan::getBorrowDate() const{
    return borrowDate;
}

// Getter for list of loans
vector<Loan>& Loan::getLoans_List(){
    return LibrarySystem::getInstance().getLoans();
}

// Setter for borrow date
void Loan::setBorrowDate(Date r) {
    borrowDate = r;
}

// Function to calculate fines
int Loan::calculateFines(){
    if(dueDate<Date::getCrrentDate()){
        int days=Date::getCrrentDate()-dueDate;
        // Cap fine at reasonable maximum (10000) to prevent overflow
        int fine = days * 2;
        if (fine > 10000) {
            cout << "Warning: Fine calculation exceeded maximum limit. Capping at 10000." << endl;
            return 10000;
        }
        return fine;
    }
    else {
        return 0;
    }
}

// Function to load loans from a file
bool Loan::loadLoans() {
    std::ifstream file(resolveDataFilePath("loaned.json"));
    if (!file.is_open()) {
        LibrarySystem::getInstance().getLoans().clear();
        return true;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        LibrarySystem::getInstance().getLoans().clear();
        file.close();
        return true;
    }

    vector<Loan> loadedLoans;

    try {
        json j;
        file >> j;

        for (const auto& loan_json : j) {
            Loan ln(loan_json["memberID"], loan_json["bookID"].get<string>(), Custom_String_Class(loan_json["dueDate"].get<string>()));
            ln.setBorrowDate(Custom_String_Class(loan_json["borrowDate"].get<string>()));
            loadedLoans.push_back(ln);
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse loaned.json: " << e.what() << std::endl;
        return false;
    }

    getLoans_List() = loadedLoans;
    file.close();
    return true;
}

// Function to save loans to a file
void Loan::saveLoans() {
    std::ofstream file(resolveDataFilePath("loaned.json"));
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << endl;
        return;
    }
    json OUTPUT;
    for(const auto& loan : getLoans_List())
    {
        json loanJSON;
        loanJSON["memberID"]=loan.memberID;
        loanJSON["bookID"] =loan.bookID.getSTR();
        loanJSON["dueDate"] =loan.dueDate.getDate().getSTR();
        loanJSON["borrowDate"] =loan.borrowDate.getDate().getSTR();
        OUTPUT.push_back(loanJSON);
    }
    file<<std::setw(4)<<OUTPUT<<std::endl; // setw(4) sets the width of the output field to 4
    file.close();
}

// Function to display the list of loans
void Loan::displaylist() {
    const auto& loans = getLoans_List();
    cout << "============================================================\n";
    cout << "All Loans\n";
    cout << "============================================================\n";
    if (loans.empty()) {
        cout << "No active loans found.\n";
        cout << "------------------------------------------------------------\n";
        return;
    }
    cout << left
         << setw(5)  << "No."
         << setw(12) << "Member ID"
         << setw(18) << "Book ISBN"
         << setw(18) << "Due Date"
         << "Borrow Date" << endl;
    cout << "------------------------------------------------------------\n";
    int i = 1;
    for (const auto &it: loans) {
        cout << left
             << setw(5)  << i++
             << setw(12) << it.getMemberID()
             << setw(18) << it.getBookID()
             << setw(18) << it.getDueDate().getDate()
             << it.getBorrowDate().getDate()
             << endl;
    }
    cout << "------------------------------------------------------------\n";
}
