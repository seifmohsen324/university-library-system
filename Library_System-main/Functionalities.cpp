// Include the necessary header files
#include "Functionalities.h"
#include <limits>
#include <stdexcept>

namespace {
void clearBadInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
}

bool readInt(int& value) {
    if (cin >> value) {
        return true;
    }
    clearBadInput();
    return false;
}

bool readIntInRange(int& value, int minValue, int maxValue) {
    if (!readInt(value)) {
        cout << "Invalid input. Please enter a number." << endl;
        return false;
    }
    if (value < minValue || value > maxValue) {
        cout << "Invalid choice! Please select a valid option." << endl;
        return false;
    }
    return true;
}

// Function to save all data to files
void saveFiles(){
    // Save all members, loans, librarians, and books
    Librarian::saveMembers();
    Loan::saveLoans();
    Librarian::saveBorrowRequests();
    Book::savelibrary();
}

// Function to load all data from files
void loadFiles(){
    // Load all members, loans, librarians, and books
    bool ok = true;
    ok = Book::loadlibrary() && ok;
    ok = Loan::loadLoans() && ok;
    ok = Librarian::loadMembers() && ok;
    ok = Librarian::loadBorrowRequests() && ok;

    if (!ok) {
        throw std::runtime_error("One or more data files failed to load");
    }
}

// Function to login a user
Custom_String_Class login(int id){
    // Get all members and librarians
    vector<Member>& member = Member::getMembers();
    vector<Librarian>& librarian = Librarian::getLibrarians();

    // Check if ID matches any member or librarian
    for(auto it:member){
        if(id==it.getID()){
            return "member";
        }
    }
    for(auto it:librarian){
        if(id==it.getID()){
            return "librarian";
        }
    }
    return "not found";
}


// Function to implement the choice of a member
void implementMemberChoice(Member& member, int memberOption){
    switch (memberOption) {
        case 1: {
            // Option 1: Borrow a book
            Book::displaylist();
            vector<Book>& books = Book::getBookList();
            if (books.empty()) {
                break;
            }
            cout << "\n Choose a book to borrow: ";
            int choice;
            if (!readIntInRange(choice, 1, static_cast<int>(books.size()))) {
                break;
            }
            Book book = books[choice - 1];
            if (!book.checkAvailability()) {
                cout << "This book is currently unavailable." << endl;
                break;
            }
            member.requestBorrow(book);
            break;
        }
        case 2: {
            // Option 2: Return a book
            member.displayloaned();
            vector<Loan> loanedbooks = member.getCheckedOutBooks();
            if (loanedbooks.empty()) {
                cout << "You do not have any active loans to return." << endl;
                break;
            }
            cout << "\n Choose a book to return: ";
            int choice;
            if (!readIntInRange(choice, 1, static_cast<int>(loanedbooks.size()))) {
                break;
            }
            Book loanedbook = Book::findByISBN(loanedbooks[choice - 1].getBookID());
            if (loanedbook.getTitle() == Book().getTitle()) {
                cout << "Could not find the selected book in the library list." << endl;
                break;
            }
            member.returnBook(loanedbook);
            break;
        }
        case 3: {
            // Option 3: Display loaned books
            member.displayloaned();
            break;
        }
        case 4: {
            // Option 4: Display member details
            member.display();
            break;
        }
        case 5: {
            // Option 5: Display all books
            Book::displaylist();
            break;
        }
        case 6: {
            // Option 6: Find a book
            displayMenuFindBook();
            int findOptions;
            if (!readIntInRange(findOptions, 1, 3)) {
                break;
            }
            implementFindBookChoice(findOptions);
            break;
        }
        default: {
            break;
        }
    }

}

// Function to implement the choice of the librarian
void implementLibrarianChoice(Librarian& librarian, int librarianOption){
    switch (librarianOption) {
        case 1:
            // Option 1: Add a book
            librarian.addBook();
            break;
        case 2: {
            // Option 2: Remove a book
            Book::displaylist();
            vector<Book>& books = Book::getBookList();
            if (books.empty()) {
                break;
            }
            int choice;
            cout << "\n Enter you choice: ";
            if (!readIntInRange(choice, 1, static_cast<int>(books.size()))) {
                break;
            }
            Book book = books[choice - 1];
            librarian.removeBook(book.getISBN());
            break;
        }
        case 3:
            // Option 3: Register a new member
            librarian.registerNewMember();
            break;
        case 4: {
            // Option 4: Remove a member
            librarian.displayAllMembers();
            int choice;
            cout << "\n Enter you choice: ";
            if (!readIntInRange(choice, 1, static_cast<int>(Member::getMembers().size()))) {
                break;
            }
            vector<Member> members= Member::getMembers();
            Member member = members[choice-1];
            librarian.removeMember(member.getID());
            break;
        }
        case 5: {
            // Option 5: Edit a book
            Book::displaylist();
            vector<Book>& books = Book::getBookList();
            if (books.empty()) {
                break;
            }
            int choice;
            cout << "\n Enter you choice: ";
            if (!readIntInRange(choice, 1, static_cast<int>(books.size()))) {
                break;
            }
            librarian.editBook(books[choice - 1]);
            break;
        }
        case 6: {
            // Option 6: Approve a borrow request
            librarian.displayRequests();
            if (Librarian::getBorrowRequests().empty()) {
                break;
            }
            int choice;
            if (!readIntInRange(choice, 1, static_cast<int>(Librarian::getBorrowRequests().size()))) {
                break;
            }
            vector<Loan>& requests = Librarian::getBorrowRequests();
            Loan loan = requests[choice - 1];
            librarian.approveBorrowRequest(loan);
            break;
        }
        case 7: {
            // Option 7: Return a book
            cout << "Enter member ID: ";
            int memberId;
            if (!readInt(memberId)) {
                cout << "Invalid input. Please enter a numeric member ID." << endl;
                break;
            }
            Member* memberPtr = nullptr;
            for (auto& m : Member::getMembers()) {
                if (m.getID() == memberId) {
                    memberPtr = &m;
                    break;
                }
            }
            if (!memberPtr) {
                cout << "Member not found." << endl;
                break;
            }
            cout << "Enter ISBN of book to return: ";
            Custom_String_Class isbn;
            cin >> isbn;
            Book book = Book::findByISBN(isbn);
            if (book.getISBN() == Book().getISBN()) {
                cout << "Book not found." << endl;
                break;
            }
            librarian.returnBook(*memberPtr, book);
            break;
        }
        case 8:
            // Option 8: Display all users using runtime polymorphism
            displayAllEntities();
            break;
        case 9:
            // Option 9: Display all books
            Book::displaylist();
            break;
        case 10:
            // Option 10: Display all requests
            librarian.displayRequests();
            break;
        case 11:
            // Option 11: Display all loans
            Loan::displaylist();
            break;
        case 12: {
            // Option 12: Find a book
            displayMenuFindBook();
            int findOptions;
            if (!readIntInRange(findOptions, 1, 3)) {
                break;
            }
            implementFindBookChoice(findOptions);
            break;
        }
        case 13: {
            // Option 13: Find a member
            displayMenuFindMember();
            int findOptions;
            if (!readIntInRange(findOptions, 1, 2)) {
                break;
            }
            implementFindMemberChoice(findOptions);
            break;
        }
        case 14: {
            // Option 14: Find a librarian
            displayMenuFindlibrarian();
            int findOptions;
            if (!readIntInRange(findOptions, 1, 2)) {
                break;
            }
            implementFindLibrarianChoice(findOptions);
            break;
        }
        default:
            break;
    }
}

// Function to implement the choice of finding a book
void implementFindBookChoice(int option){
    switch (option) {
        case 1: {
            // Option 1: Find a book by ISBN
            cout << "enter the ISBN of the book you want to find" << endl;
            Custom_String_Class isbn;
            cin >> isbn;
            Book bk = Book::findByISBN(isbn);
            if (!(bk.getTitle() == Book().getTitle()))
                bk.displayBook();
            break;
        }
        case 2: {
            // Option 2: Find a book by title
            cout << "enter the title of the book you want to find" << endl;
            Custom_String_Class title;
            cin >> title;
            Book bk = Book::findByName(title);
            if (!(bk.getTitle() == Book().getTitle()))
                bk.displayBook();
            break;
        }
        case 3: {
            // Option 3: Find a book by author
            cout << "enter the name of author of the book you want to find" << endl;
            Custom_String_Class author;
            cin >> author;
            Book bk = Book::findByAuthor(author);
            if (!(bk.getTitle() == Book().getTitle()))
                bk.displayBook();
            break;
        }
        default: {
            break;
        }
    }
}

// Function to implement the choice of finding a member
void implementFindMemberChoice(int option){
    switch (option) {
        case 1: {
            // Option 1: Find a member by name
            cout << "enter the name of the member you want to find" << endl;
            Custom_String_Class name;
            cin >> name;
            Member member = Librarian::findMemberByName(name);
            cout<<"\n";
            if (!(member.getName() == Member().getName()))
                member.display();
            break;
        }
        case 2: {
            // Option 2: Find a member by ID
            cout << "enter the ID of the member you want to find" << endl;
            int id;
            if (!readInt(id)) {
                cout << "Invalid input. Please enter a numeric member ID." << endl;
                break;
            }
            Member member = Librarian::findMemberByID(id);
            cout<<"\n";
            if (!(member.getName() == Member().getName()))
                member.display();
            break;
        }
        default: {
            break;
        }
    }
}

// Function to implement the choice of finding a librarian
void implementFindLibrarianChoice(int option){
    switch (option) {
        case 1: {
            // Option 1: Find a librarian by name
            cout << "enter the name of the librarian you want to find" << endl;
            Custom_String_Class name;
            cin >> name;
            Librarian librarian = Librarian::findLibrarianByName(name);
            cout<<"\n";
            if (!(librarian.getName() == Librarian().getName()))
                librarian.display();
            break;
        }
        case 2: {
            // Option 2: Find a librarian by ID
            cout << "enter the ID of the librarian you want to find" << endl;
            int id;
            if (!readInt(id)) {
                cout << "Invalid input. Please enter a numeric librarian ID." << endl;
                break;
            }
            Librarian librarian = Librarian::findLibrarianByID(id);
            cout<<"\n";
            if (!(librarian.getName() == Librarian().getName()))
                librarian.display();
            break;
        }
        default: {
            break;
        }
    }
}



void displayCompact(const Member& member) {
    cout << "Member card: " << member.getName()
         << " | ID " << member.getID()
         << " | Limit " << member.getBorrowLimit() << endl;
}

void displayCompact(const Librarian& librarian) {
    cout << "Librarian card: " << librarian.getName()
         << " | ID " << librarian.getID()
         << " | Privileges " << librarian.getBorrowLimit() << endl;
}

// displayAllEntities() shows both compile-time and runtime polymorphism.
void displayAllEntities() {
    cout << "Compile-time polymorphism sample:\n";
    for (const auto& member : Member::getMembers()) {
        displayCompact(member);
    }
    for (const auto& librarian : Librarian::getLibrarians()) {
        displayCompact(librarian);
    }

    cout << "\nRuntime polymorphism sample:\n";
    vector<LibraryEntity*> everyone;

    // Add all members through base pointer
    for (auto& m : Member::getMembers())
        everyone.push_back(&m);

    // Add all librarians through base pointer
    for (auto& l : Librarian::getLibrarians())
        everyone.push_back(&l);

    cout << "--------------------\n";
    cout << "All Library Users:\n";
    cout << "--------------------\n";

    // Virtual dispatch â€” correct display() called based on actual type
    for (LibraryEntity* entity : everyone) {
        cout << "Role: " << entity->getRole() << endl;
        cout << "Borrow Limit: " << entity->getBorrowLimit() << endl;
        entity->display();
    }
}

