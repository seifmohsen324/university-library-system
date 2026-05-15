#include "LibrarySystem.h"
using namespace std;

// Get all books
const vector<Book>& LibrarySystem::getBooks() const {
    return books;
}

// Get all books (mutable)
vector<Book>& LibrarySystem::getBooks() {
    return books;
}

// Get all members
const vector<Member>& LibrarySystem::getMembers() const {
    return members;
}

// Get all members (mutable)
vector<Member>& LibrarySystem::getMembers() {
    return members;
}

// Get all loans
const vector<Loan>& LibrarySystem::getLoans() const {
    return loans;
}

// Get all loans (mutable)
vector<Loan>& LibrarySystem::getLoans() {
    return loans;
}

const vector<Librarian>& LibrarySystem::getLibrarians() const {
    return librarians;
}

vector<Librarian>& LibrarySystem::getLibrarians() {
    return librarians;
}

vector<Loan>& LibrarySystem::getBorrowRequests() {
    return borrowRequests;
}
