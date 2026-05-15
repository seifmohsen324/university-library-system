#include "Book.h"
#include "LibrarySystem.h"
#include "DataPaths.h"
#include "Functionalities.h"
#include <iomanip>
#include <chrono>
#include <ctime>

namespace {
int currentYear() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&t);
    return local ? (local->tm_year + 1900) : 2026;
}
}

// Default constructor initializing an empty book
Book::Book() : Title(""), Author(""), ISBN(""), Genre(""), Quantity(0), Publication_Year(0) {}

// Parameterized constructor to create a book with given details
Book::Book(const Custom_String_Class& tit, const Custom_String_Class& Auth, const Custom_String_Class& ISB, const Custom_String_Class& Gen, int Pub_yr, int Quant)
        : Title(tit), Author(Auth), ISBN(ISB), Genre(Gen), Quantity(Quant), Publication_Year(Pub_yr) {}

// Function to display book details
void Book::displayBook() const {
    cout << "============================================================\n";
    cout << "Book Details\n";
    cout << "============================================================\n";
    cout << left << setw(22) << "Title:" << Title << endl;
    cout << left << setw(22) << "Author:" << Author << endl;
    cout << left << setw(22) << "ISBN:" << ISBN << endl;
    cout << left << setw(22) << "Publication Year:" << Publication_Year << endl;
    cout << left << setw(22) << "Genre:" << Genre << endl;
    cout << left << setw(22) << "Collection:" << (isRare() ? "Rare Book" : "Regular Book") << endl;
    cout << left << setw(22) << "Availability:" << (checkAvailability() ? "Available" : "Checked out") << endl;
    cout << left << setw(22) << "Quantity:" << Quantity << endl;
    cout << "------------------------------------------------------------\n";
}

// Function to display the list of all books
void Book::displaylist() {
    int i = 1;
    cout << "============================================================\n";
    cout << "All Books in the Library\n";
    cout << "============================================================\n";
    cout << left
         << setw(5)  << "No."
         << setw(30) << "Title"
         << setw(24) << "Author"
         << setw(18) << "ISBN"
         << setw(6)  << "Qty"
         << "Status" << endl;
    cout << "------------------------------------------------------------\n";
    bool foundBook = false;
    for (const auto& it : LibrarySystem::getInstance().getBooks()) {
        foundBook = true;
        cout << left
             << setw(5)  << i++
             << setw(30) << it.Title
             << setw(24) << it.Author
             << setw(18) << it.ISBN
             << setw(6)  << it.Quantity
             << (it.checkAvailability() ? "Available" : "Checked out")
             << " | " << (it.isRare() ? "Rare" : "Regular")
             << endl;
    }
    if (!foundBook) {
        cout << "No books are currently available in the library." << endl;
    }
    cout << "============================================================\n";
}



// Function to load the library from a JSON file
bool Book::loadlibrary() {
    std::ifstream file(resolveDataFilePath("books.json"));//opening books.json which has all book information
    if (!file.is_open()) {//making sure books.json was opened successfully
        LibrarySystem::getInstance().getBooks().clear();
        return true;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        LibrarySystem::getInstance().getBooks().clear();
        file.close();
        return true;
    }

    vector<Book> loadedBooks;

    try {
        json j;//creating a variable of type json which can hold json data
        file >> j;//moving data from file to j

        for (const auto& book_json : j) {//iterating over j(book data)
            Book bk;//creating a temporary book to push in book list
            //getting member attributes from j
            bk.Title = (book_json["title"].get<string>());
            bk.Author = book_json["author"].get<string>();
            bk.ISBN = book_json["isbn"].get<string>();
            bk.Genre = book_json["genre"].get<string>();
            bk.Publication_Year = book_json["publication_year"];
            bk.Quantity = book_json["quantity"];
            loadedBooks.push_back(bk);//pushing book into temporary list
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse books.json: " << e.what() << std::endl;
        return false;
    }

    LibrarySystem::getInstance().getBooks() = loadedBooks;
    file.close();//closing the file
    return true;
}

// Function to save the library to a JSON file
bool Book::savelibrary() {
    std::ofstream file(resolveDataFilePath("books.json"));//same as load
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << endl;
        return false;
    }

    json OUTPUT;
    for (const auto& book : LibrarySystem::getInstance().getBooks()) {//iterating over the singleton list
        json bookJson;
        //creating json keys and assigning values to it from book list
        bookJson["title"] = book.Title.getSTR();
        bookJson["author"] = book.Author.getSTR();
        bookJson["publication_year"] = book.Publication_Year;
        bookJson["isbn"] = book.ISBN.getSTR();
        bookJson["genre"] = book.Genre.getSTR();
        bookJson["quantity"] = book.Quantity;
        OUTPUT.push_back(bookJson);
    }
    file << setw(4) << OUTPUT << endl;//writing to Book.json .. setw(4) is just to maintain the format of the file
    file.close();
    return true;
}

// Getters and setters for book attributes
Custom_String_Class& Book::getISBN() { return ISBN; }
Custom_String_Class& Book::getTitle() { return Title; }
Custom_String_Class& Book::getAuthor() { return Author; }
Custom_String_Class& Book::getGenre() { return Genre; }
unsigned int& Book::getQuantity() { return Quantity; }
unsigned int Book::getPublicationYear() const { return Publication_Year; }
vector<Book>& Book::getBookList() { return LibrarySystem::getInstance().getBooks(); }
void Book::setISBN(const Custom_String_Class& newISBN) { ISBN = newISBN; }
void Book::setPubYear(unsigned int newPubYear) { Publication_Year = newPubYear; }
void Book::setQuantity(unsigned int newQuantity) { Quantity = newQuantity; }

// Function to check if a book is available
bool Book::checkAvailability() const { return (Quantity > 0); }

// Function to check if a book is rare
bool Book::isRare() const {
    const int ageThreshold = 75;
    return Publication_Year > 0 && Publication_Year <= static_cast<unsigned int>(currentYear() - ageThreshold);
}

// Function to find a book by author
Book Book::findByAuthor(Custom_String_Class name) {
    vector<Book> results;//vector of books to hold search results
    int found = 0;//counts books found
    cout << "found :" << endl;
    for (auto& it : LibrarySystem::getInstance().getBooks()) {//iterating over book list and searching for the name sent to function
        if (it.getAuthor().find(name)) {
            found++;//incrementing number of found books
            cout << found << ":" << it.getTitle() << endl;//printing the found book title
            results.push_back(it);//pushing it in the results vector which the user will choose a book from
        }
    }
    if (!found) {
        cout << "Couldnt Find Any Results For : " << name << endl;
        return Book();//if we didn't find a book we return the default book which has NULL data members
    }

    while (true) {
        cout << "Pick a Book by Number :";
        int choice;
        if (!readIntInRange(choice, 1, found)) {
            continue;
        }
        return results[choice - 1];//return the book
    }
}

// Function to find a book by name
Book Book::findByName(Custom_String_Class name) {//this function is nearly identical to findByAuthor, but it searches by book name instead
    vector<Book> results;
    int found = 0;
    cout << "found :" << endl;
    for (auto& it : LibrarySystem::getInstance().getBooks()) {
        if (it.getTitle().find(name)) {
            found++;
            cout << found << ":" << it.getTitle() << endl;
            results.push_back(it);
        }
    }
    if (!found) {
        cout << "couldnt find a book \n";
        return Book();
    }

    while (true) {
        cout << "Pick a Book by Number :";
        int choice;
        if (!readIntInRange(choice, 1, found)) {
            continue;
        }
        return results[choice - 1];
    }
}

// Function to find a book by ISBN
Book Book::findByISBN(Custom_String_Class ID) {
    for (auto& it : LibrarySystem::getInstance().getBooks()) {//iterating over the singleton list
        if (it.getISBN() == ID) {//if sent id is equal to somebook id return that book
            return it;
        }
    }
    cout << "Couldnt Find a Book" << endl;
    return Book();//if not found return the default book
}

// Operator overloading: allows cout << book to print book details
ostream& operator<<(ostream& os, const Book& b) {
    os << "--------------------\n";
    os << "Title:  " << b.Title  << "\n";
    os << "Author: " << b.Author << "\n";
    os << "ISBN:   " << b.ISBN   << "\n";
    os << "Genre:  " << b.Genre  << "\n";
    os << "Year:   " << b.Publication_Year << "\n";
    os << "Collection: " << (b.isRare() ? "Rare Book" : "Regular Book") << "\n";
    os << "Qty:    " << b.Quantity << "\n";
    os << "--------------------\n";
    return os;
}
