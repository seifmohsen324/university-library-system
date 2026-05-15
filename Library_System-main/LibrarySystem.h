#pragma once

#ifndef LIBRARY_SYSTEM_H
#define LIBRARY_SYSTEM_H

#include <vector>
#include "Book.h"
#include "User.h"
#include "Loan.h"
using namespace std;
class LibrarySystem {
private:
    // Private constructor to prevent direct instantiation
    LibrarySystem() {}

    // Private destructor to prevent deletion
    ~LibrarySystem() = default;

    // Data members
    vector<Book> books;
    vector<Member> members;
    vector<Loan> loans;
    vector<Librarian> librarians;
    vector<Loan> borrowRequests;

public:
    // Delete copy, move constructors and assignment operators
    LibrarySystem(const LibrarySystem&) = delete;
    LibrarySystem& operator=(const LibrarySystem&) = delete;
    LibrarySystem(LibrarySystem&&) = delete;
    LibrarySystem& operator=(LibrarySystem&&) = delete;

    // Static method to get the single instance (Thread-safe)
    static LibrarySystem& getInstance() {
        static LibrarySystem instance; // C++11 guarantees thread-safe initialization
        return instance;
    }

    const vector<Book>& getBooks() const;
    vector<Book>& getBooks();

    const vector<Member>& getMembers() const;
    vector<Member>& getMembers();

    const vector<Loan>& getLoans() const;
    vector<Loan>& getLoans();

    const vector<Librarian>& getLibrarians() const;
    vector<Librarian>& getLibrarians();

    vector<Loan>& getBorrowRequests();

};

#endif
