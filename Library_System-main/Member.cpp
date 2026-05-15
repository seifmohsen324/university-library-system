#include "User.h"
#include "LibrarySystem.h"
#include <iomanip>
#include <cctype>
using namespace std;

// Use nlohmann's json library
using json = nlohmann::json;

// Default constructor
Member::Member():Fines(0),Name(""),ID(0),Type("") {}

// Constructor with name, ID, and type. Fines are set to 0.
Member::Member(const Custom_String_Class& N,int I,const Custom_String_Class& T): Name(N),ID(I),Type(T){
    Fines=0;
}

// Getter for name
Custom_String_Class Member::getName() const{
    return Name;
}

// Getter for list of members
vector<Member>& Member::getMembers(){
    return LibrarySystem::getInstance().getMembers();
}

// Getter for ID
int Member::getID() const {
    return ID;
}

// Getter for type
Custom_String_Class Member::getType() const{
    return Type;
}

// Getter for fines
int Member::getFines() const{
    return Fines;
}

// Getter for checked out books
vector<Loan>& Member::getCheckedOutBooks(){
    return checkedOutBooks;
}

// Setter for name
void Member::setName(const Custom_String_Class& name) {
    Name = name;
}

// Setter for ID
void Member::setID(int id) {
    ID = id;
}

// Setter for type
void Member::setType(const Custom_String_Class& type) {
    Type = type;
}

// Setter for fines
void Member::setFines(int f){
    Fines=f;
}

// Function to calculate total fines
int Member::calculateTotalFines (){
    Fines=0;
    for(auto it: checkedOutBooks){//iterate over the list of checkedout books for a member and calculate their total fines
        Fines+=it.calculateFines();
    }
    return Fines;
}

// Function to update total fines
void Member::updateTotalFines (){
    Fines=calculateTotalFines();
}

// Function to display member details
void Member::display(){
    cout << "============================================================\n";
    cout << "Member Details\n";
    cout << "============================================================\n";
    std::cout << left << setw(24) << "Name:" << Name << std::endl;
    std::cout << left << setw(24) << "ID:" << ID << std::endl;
    std::cout << left << setw(24) << "Type:" << Type << std::endl;
    std::cout << left << setw(24) << "Borrow Limit:" << getBorrowLimit() << std::endl;
    std::cout << left << setw(24) << "Checked Out Books:" << checkedOutBooks.size() << std::endl;
    std::cout << left << setw(24) << "Overdue Fines:" << calculateTotalFines() << std::endl;
    cout<< "------------------------------------------------------------\n";
}

// Function to display loaned books
void Member::displayloaned() const{
    cout << "============================================================\n";
    cout << "Loaned Books\n";
    cout << "============================================================\n";
    if (checkedOutBooks.empty()) {
        cout << "No books are currently borrowed.\n";
        cout << "------------------------------------------------------------\n";
        return;
    }
    cout << left
         << setw(5)  << "No."
         << setw(18) << "Book ISBN"
         << setw(18) << "Due Date"
         << "Borrow Date" << endl;
    cout << "------------------------------------------------------------\n";
    for (size_t i = 0; i < this->checkedOutBooks.size(); ++i) {
        const Loan& loan = this->checkedOutBooks[i];
        cout << left
             << setw(5)  << (i + 1)
             << setw(18) << loan.getBookID()
             << setw(18) << loan.getDueDate().getDate()
             << loan.getBorrowDate().getDate() << endl;
    }
    cout << "------------------------------------------------------------\n";
}

// Function to request to borrow a book
void Member::requestBorrow(Book& book) {
    if (!book.checkAvailability()) {
        cout << "This book is currently unavailable." << endl;
        return;
    }

    int pendingRequests = 0;
    for (const auto& request : Librarian::getBorrowRequests()) {
        if (request.getMemberID() == ID) {
            ++pendingRequests;
        }
    }

    if (static_cast<int>(checkedOutBooks.size()) + pendingRequests >= getBorrowLimit()) {
        cout << "You have reached your borrow limit." << endl;
        return;
    }

    for (const auto& loan : checkedOutBooks) {
        if (loan.getBookID() == book.getISBN()) {
            cout << "You already have this book checked out." << endl;
            return;
        }
    }

    for (const auto& request : Librarian::getBorrowRequests()) {
        if (request.getMemberID() == ID && request.getBookID() == book.getISBN()) {
            cout << "You already have a pending request for this book." << endl;
            return;
        }
    }

    Loan ln(ID, book.getISBN());//create a new loan
    Librarian::addBorrowRequest(ln);//send loan request to the library
    cout << "Borrow request submitted successfully." << endl;
}

// Function to return a book
void Member::returnBook(Book book) {
    Librarian::returnBook(*this, book);//send return request to library which will be processed automatically
}

// Returns the member's role — used by LibraryEntity polymorphism
Custom_String_Class Member::getRole() const {
    return Type;
}

int Member::getBorrowLimit() const {
    Custom_String_Class loweredType = Type;
    std::string typeText = loweredType.toString();
    for (char& ch : typeText) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }

    if (typeText == "student" || typeText == "member") return 3;
    if (typeText == "staff") return 5;
    if (typeText == "faculty") return 7;
    return 0;
}
