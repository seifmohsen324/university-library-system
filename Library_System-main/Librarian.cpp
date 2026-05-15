#include "User.h" // Include the User header file
#include "UserFactory.h"  //factory design pattern
#include "LibrarySystem.h"
#include "DataPaths.h"
#include "Functionalities.h"
#include <algorithm>
#include <cctype>

namespace {
Custom_String_Class normalizeType(const Custom_String_Class& type) {
    std::string lowered = type.toString();
    std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return Custom_String_Class(lowered);
}

Custom_String_Class canonicalMemberType(const Custom_String_Class& normalizedType) {
    if (normalizedType == "member") return Custom_String_Class("Member");
    if (normalizedType == "student") return Custom_String_Class("Student");
    if (normalizedType == "staff") return Custom_String_Class("Staff");
    if (normalizedType == "faculty") return Custom_String_Class("Faculty");
    return Custom_String_Class("");
}
}

// Default constructor for Librarian class
Librarian::Librarian():Member() {}

// Parameterized constructor for Librarian class
Librarian::Librarian(const Custom_String_Class& name, int ID) : Member(name, ID, Custom_String_Class("Librarian")) {}

// Function to add a book to the library inventory
void Librarian::addBook() {
    Custom_String_Class title;
    cout << "Enter book's title: " << endl;
    cin >> title;

    Custom_String_Class isbn;
    cout << "Enter book's ISBN: " << endl;
    cin >> isbn;

    Custom_String_Class author;
    cout << "Enter author's name: " << endl;
    cin >> author;

    Custom_String_Class genre;
    cout << "Enter book's genre: " << endl;
    cin >> genre;

    int publicationYear;
    cout << "Enter book's publication year: " << endl;
    cin >> publicationYear;
    
    // Validate publication year
    if (publicationYear < 1000 || publicationYear > 2100) {
        cout << "Error: Publication year must be between 1000 and 2100!" << endl;
        return;
    }

    int Quantity;
    cout<<"Enter Book's Quantity: ";
    cin>>Quantity;
    
    // Validate quantity
    if (Quantity <= 0) {
        cout << "Error: Book quantity must be greater than 0!" << endl;
        return;
    }

    for (auto& existingBook : Book::getBookList()) {
        if (existingBook.getISBN() == isbn) {
            cout << "Error: A book with this ISBN already exists!" << endl;
            return;
        }
    }

    auto newBook = UserFactory::createBook(title, author, isbn,
    genre, publicationYear, Quantity);
 if (!newBook) {
    cout << "Invalid book data. Book was not added.\n";
    return;
}
Book::getBookList().push_back(*newBook);
cout << "Book added successfully.\n";
}

// Function to remove a book from the library inventory
void Librarian::removeBook(const Custom_String_Class& ISBN) {
    // Check if the book is currently loaned out
    for(auto it : Loan::getLoans_List()){
        if(ISBN==it.getBookID()){
            cout<<"the book is loaned by someone you can't remove it";
            return;
        }
    }

    // Remove the book if it's not loaned out
    for(auto it = Book::getBookList().begin(); it != Book::getBookList().end(); ) {
        if(it->getISBN() == ISBN) {
            cout << "Now I am removing \n";
            Book::getBookList().erase(it); // Erase the book from the book list
            return;
        } else {
            ++it; // Move to the next element
        }
    }
}

// Function to add a borrow request to the list
void Librarian::addBorrowRequest(Loan& ln) {
    LibrarySystem::getInstance().getBorrowRequests().push_back(ln); 
}

// Function to display all pending borrow requests
void Librarian::displayRequests() {
    auto& requests = LibrarySystem::getInstance().getBorrowRequests();
    if (requests.empty()) {
        cout << "No pending borrow requests." << std::endl;
        return;
    }

    cout << "Pending Borrow Requests:\n";
    for (size_t i = 0; i < requests.size(); ++i) {
        const auto& request = requests[i];
        cout << i + 1 << ". Member ID: " << request.getMemberID()
             << ", Book ISBN: " << request.getBookID() << std::endl;
    }
}

// Function to process a borrow request for a book
bool Librarian::borrowBook(Book b, Member& member){
    vector<Book>& bkList= Book::getBookList(); // Get the list of all books
    vector<Loan>& checkedBooks= member.getCheckedOutBooks(); // Get the list of books checked out by the member
    vector<Loan>& loansList= Loan::getLoans_List(); // Get the list of all loans

    auto bookIt = std::find_if(bkList.begin(), bkList.end(), [&](Book& item) {
        return item.getISBN() == b.getISBN();
    });
    if (bookIt == bkList.end()) {
        cout << "Could not find the requested book in the library list." << endl;
        return false;
    }

    // Validate the member type before changing inventory.
    Custom_String_Class memberType = normalizeType(member.getType());
    Date dueDate;
    if(memberType == "member" || memberType == "student") {
        dueDate = Date::getCrrentDate() + 7;
    }
    else if(memberType=="staff")
        dueDate = Date::getCrrentDate() + 10;
    else if(memberType=="faculty")
        dueDate = Date::getCrrentDate() + 14;
    else {
        cout << "Unknown member type. Cannot determine loan duration." << endl;
        return false;
    }

    if (static_cast<int>(checkedBooks.size()) >= member.getBorrowLimit()) {
        cout << "Member has reached the borrow limit." << endl;
        return false;
    }

    for (const auto& loan : checkedBooks) {
        if (loan.getBookID() == bookIt->getISBN()) {
            cout << "Member already has this book checked out." << endl;
            return false;
        }
    }

    // Check if the book is available
    if(bookIt->getQuantity()>0) {
        // If only one copy is left, set the book as unavailable
        // Decrease the quantity of the book
        bookIt->setQuantity(bookIt->getQuantity()-1);

        Loan newloan(member.getID(), bookIt->getISBN(), dueDate, Date::getCrrentDate());         // Create a new loan
        checkedBooks.push_back(newloan);         // Add the loan to the member's checked out books
        loansList.push_back(newloan);         // Add the loan to the list of all loans
        return true;
    }
    else{ // If the book is not available
        cout << "there are no copies of the book available"<<endl;
        return false;
    }
}

// Function to approve a borrow request and issue the book
void Librarian::approveBorrowRequest(Loan ln) {
    // Find the actual member in the Singleton list to update their records
    auto& memberList = Member::getMembers();
    Member* memberPtr = nullptr;
    for(auto& m : memberList) {
        if(m.getID() == ln.getMemberID()) {
            memberPtr = &m;
            break;
        }
    }

    if (!memberPtr) {
        cout << "Member not found for this request." << endl;
        return;
    }

    Book bk = Book::findByISBN(ln.getBookID());
    Book defaultBook;
    if (bk.getISBN() == defaultBook.getISBN()) {
        cout << "Requested book was not found." << endl;
        return;
    }
    auto& requests = LibrarySystem::getInstance().getBorrowRequests();
    if(borrowBook(bk, *memberPtr)) {
        for(auto it = requests.begin(); it != requests.end(); ) {
            if(it->getBookID() == ln.getBookID() && it->getMemberID() ==ln.getMemberID()) {
                requests.erase(it);
                break;
            } else {
                ++it;
            }
        }
        cout << "Borrow request approved and book issued." << endl;
    }
}

// Function to return a borrowed book to the library
void Librarian::returnBook(Member& member, Book& book) {
    vector<Book>& bkList= Book::getBookList();
    vector<Loan>& checkedBooks= member.getCheckedOutBooks();
    vector<Loan>& loansList= Loan::getLoans_List();

    auto bookIt = std::find_if(bkList.begin(), bkList.end(), [&](Book& item) {
        return item.getISBN() == book.getISBN();
    });
    if (bookIt == bkList.end()) {
        cout << "Could not find the book in the library list." << endl;
        return;
    }

    auto memberLoanIt = std::find_if(checkedBooks.begin(), checkedBooks.end(), [&](const Loan& loan) {
        return (loan.getBookID() == bookIt->getISBN()) && (loan.getMemberID() == member.getID());
    });
    if (memberLoanIt == checkedBooks.end()) {
        cout << "This member does not have that book checked out." << endl;
        return;
    }

    bookIt->setQuantity(bookIt->getQuantity()+1);
    checkedBooks.erase(memberLoanIt); // Remove the book from the checked out books list
    for(auto it =loansList.begin(); it != loansList.end() ; ) {
        if((it->getBookID() == bookIt->getISBN()) && (it->getMemberID()==member.getID())) {
            Custom_String_Class tempISBN = it->getBookID();
            loansList.erase(it); 
            cout<<"\nRemoved Book With ISBN("<<tempISBN<<")\n";
            break;
        } else {
            ++it;
        }
    }
}

// Function to edit the details of a book in the library inventory
void Librarian::editBook(Book& book)
{
    char answer;
    cout << "Do you want to edit the title ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        cout << "Enter book's title: " << endl;
        cin >> book.getTitle();
    }
    cout << "Do you want to edit the ISBN ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        bool canEditISBN = true;
        for (const auto& loan : Loan::getLoans_List()) {
            if (loan.getBookID() == book.getISBN()) {
                cout << "This book is currently loaned. ISBN cannot be changed until all copies are returned." << endl;
                canEditISBN = false;
                break;
            }
        }

        if (canEditISBN) {
            cout << "Enter book's ISBN: " << endl;
            Custom_String_Class newISBN;
            cin >> newISBN;
            bool duplicate = false;
            for (auto& existingBook : Book::getBookList()) {
                if (&existingBook != &book && existingBook.getISBN() == newISBN) {
                    duplicate = true;
                    break;
                }
            }
            if (duplicate) {
                cout << "A book with this ISBN already exists. Keeping the current ISBN." << endl;
            } else if (newISBN.getSTR()[0] == '\0') {
                cout << "ISBN cannot be empty. Keeping the current ISBN." << endl;
            } else {
                book.setISBN(newISBN);
            }
        }
    }

    cout << "Do you want to edit the author ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        cout << "Enter author's name: " << endl;
        cin >> book.getAuthor();
    }

    cout << "Do you want to edit the genre ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        cout << "Enter book's genre: " << endl;
        cin >> book.getGenre();
    }

    cout << "Do you want to edit the publication year ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        cout << "Enter book's publication year: " << endl;
        int year;
        cin >> year;
        if (year < 1000 || year > 2100) {
            cout << "Invalid year. Keeping the current publication year." << endl;
        } else {
            book.setPubYear(static_cast<unsigned int>(year));
        }
    }
    cout << "Do you want to edit the quantity ?(Y/N)" << endl;
    cin >> answer;
    if(answer == 'Y')
    {
        cout<<"Enter Book's Quantity:";
        int quantity;
        cin>>quantity;
        if (quantity < 0) {
            cout << "Quantity cannot be negative. Keeping the current quantity." << endl;
        } else {
            book.setQuantity(static_cast<unsigned int>(quantity));
        }
    }
}

// Function to register a new member in the library system
void Librarian::registerNewMember(){
    Custom_String_Class name, type;
    int id;
    cout << "Enter name: ";
    cin >> name;
    cout << "Enter ID: ";
    cin >> id;
    
    // Validate ID is positive
    if (id <= 0) {
        cout << "Error: Member ID must be greater than 0!" << endl;
        return;
    }
    
    cout << "Enter type: ";
    cin >> type;

    Custom_String_Class normalizedType = normalizeType(type);
    if (normalizedType == "librarian") {
        cout << "Librarian accounts cannot be registered from the member menu.\n";
        return;
    }

    Custom_String_Class canonicalType = canonicalMemberType(normalizedType);
    if (canonicalType.getSTR()[0] == '\0') {
        cout << "Invalid member type. Registration failed.\n";
        return;
    }

    for (const auto& member : Member::getMembers()) {
        if (member.getID() == id) {
            cout << "Error: Member with ID " << id << " already exists!" << endl;
            return;
        }
    }
    for (const auto& librarian : LibrarySystem::getInstance().getLibrarians()) {
        if (librarian.getID() == id) {
            cout << "Error: A librarian with ID " << id << " already exists!" << endl;
            return;
        }
    }

    auto newUser = UserFactory::createUser(canonicalType, name, id);
    if (!newUser) {
        cout << "Invalid member type. Registration failed.\n";
        return;
    }
    Member::getMembers().push_back(*newUser);
    cout << "New member registered successfully.\n";
}


// Function to remove a member from the library
void Librarian::removeMember(int id)
{
    // Loop through all loans
    for(auto it : Loan::getLoans_List())
    {
        // If the member has not returned a loaned book yet
        if(it.getMemberID()==id)
        {
            cout<<"This Member Has Not Returned A Loaned Book Yet!\n";
            return;
        }
    }
    bool found = false;
    auto& memberList = Member::getMembers();
    // Loop through all members
    for (auto it = memberList.begin(); it != memberList.end() ; ++it) {
        // If the member is found
        if (it->getID() == id) {
            // Remove the member
            memberList.erase(it);
            cout << "Member with ID " << id << " removed successfully.\n";
            found = true;
            break;
        }
    }
    // If the member is not found
    if (!found) {
        cout << "Member not found.\n";
    }
}

// Function to display all members
void Librarian::displayAllMembers() {
    int i=1;

    std::cout << "List of Members:\n";

    cout<< "--------------------\n";
    // Loop through all members
    for (auto& member : Member::getMembers()) {
        cout<<i++<<" ";
        // Display each member
        member.display();
    }

}

// Function to display the librarian
void Librarian::display(){
    cout << "--------------------\n";
    cout << "Name: " << getName() << endl;
    cout << "ID: "   << getID()   << endl;
    cout << "Role: Librarian"     << endl;
    cout << "Borrow Limit: " << getBorrowLimit() << endl;
    cout << "Managed Members: " << Member::getMembers().size() << endl;
    cout << "Pending Requests: " << getBorrowRequests().size() << endl;
    cout << "--------------------\n";
}

// Function to load borrow requests
bool Librarian::loadBorrowRequests() {
    std::ifstream file(resolveDataFilePath("borrowRequests.json"));
    // If the file is not open
    if (!file.is_open()) {
        getBorrowRequests().clear();
        return true;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        getBorrowRequests().clear();
        file.close();
        return true;
    }

    vector<Loan> loadedRequests;

    try {
        json j;
        file >> j;

        // Loop through all requests
        for (const auto& Requests_json : j) {
            // Create a new loan
            Loan ln(Requests_json["memberID"], Custom_String_Class(Requests_json["bookID"].get<string>()), Date(Custom_String_Class(Requests_json["dueDate"].get<string>())), Date(Custom_String_Class(Requests_json["borrowDate"].get<string>())));
            // Add the loan to the temporary list
            loadedRequests.push_back(ln);
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse borrowRequests.json: " << e.what() << std::endl;
        return false;
    }

    getBorrowRequests() = loadedRequests;
    file.close();
    return true;
}

// Function to save borrow requests
bool Librarian::saveBorrowRequests() {
    std::ofstream file(resolveDataFilePath("borrowRequests.json"));
    // If the file is not open
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << endl;
        return false;
    }
    json OUTPUT;
    // Loop through all borrow requests
    for(const auto& ln : getBorrowRequests())
    {
        json bookJson;

        // Save the member ID, book ID, due date, and borrow date
        bookJson["memberID"] = ln.getMemberID();
        bookJson["bookID"] = ln.getBookID().getSTR();
        bookJson["dueDate"] = ln.getDueDate().getDate().getSTR();
        bookJson["borrowDate"] = ln.getBorrowDate().getDate().getSTR();

        // Add the book to the output
        OUTPUT.push_back(bookJson);
    }
    // Write the output to the file
    file<<setw(4)<<OUTPUT<<endl;// setw(4) sets the width of the output field to 4
    file.close();
    return true;
}

// Function to find a librarian by name
Librarian Librarian::findLibrarianByName(Custom_String_Class name){
    vector<Librarian> results;
    int found = 0;
    cout << "found :" << endl;
    // Loop through all librarians
    for (auto it : LibrarySystem::getInstance().getLibrarians()) {
        // If the librarian is found
        if (it.getName().find(name)) {
            found++;
            cout << found << ":" << it.getName() << endl;
            // Add the librarian to the results
            results.push_back(it);
        }
    }
    // If the librarian is not found
    if (!found) {
        cout << "Couldn't Find Any Results For : " << name << endl;
        return Librarian();
    }

    while (true) {
        cout << "Pick a librarian by Number :";
        int choice;
        if (!readIntInRange(choice, 1, found)) {
            continue;
        }
        return results[choice - 1];
    }
}

// Function to find a librarian by ID
Librarian Librarian::findLibrarianByID(int id){
    // Loop through all librarians
    for(auto it : LibrarySystem::getInstance().getLibrarians())
    {
        // If the librarian is found
        if(it.getID()==id)
        {
            return it;
        }
    }
    cout<<"Couldn't Find librarian"<<endl;

    return Librarian();
}

// Function to find a member by name
Member Librarian::findMemberByName(Custom_String_Class name){
    vector<Member> results;
    int found = 0;
    cout << "found :" << endl;
    // Loop through all members
    for (auto it : Member::getMembers()) {
        // If the member is found
        if (it.getName().find(name)) {
            found++;
            cout << found << ":" << it.getName() << endl;
            // Add the member to the results
            results.push_back(it);
        }
    }
    // If the member is not found
    if (!found) {
        cout << "Couldn't Find Any Results For : " << name << endl;
        return Member();
    }

    while (true) {
        cout << "Pick a member by Number :";
        int choice;
        if (!readIntInRange(choice, 1, found)) {
            continue;
        }
        return results[choice - 1];
    }
}

// Function to find a member by ID
Member Librarian::findMemberByID(int id){
    // Loop through all members
    for(auto it : Member::getMembers())
    {
        // If the member is found
        if(it.getID()==id)
        {
            return it;
        }
    }
    cout<<"Couldn't Find member"<<endl;

    return Member();
}

// Function to load members
bool Librarian::loadMembers() {
    std::ifstream file(resolveDataFilePath("users.json"));    // If the file is not open
    if (!file.is_open()) {
        Member::getMembers().clear();
        getLibrarians().clear();
        return true;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        Member::getMembers().clear();
        getLibrarians().clear();
        file.close();
        return true;
    }

    vector<Member> loadedMembers;
    vector<Librarian> loadedLibrarians;

    try {
        json j;
        file >> j;

        // Loop through all members
        for (const auto& member_json : j) {
            // If the member is not a librarian
            if(member_json["Type"].get<string>()!="Librarian") {
                Member m;

                // Set the name, ID, type, and fines of the member
                m.setName(member_json["Name"].get<string>());
                m.setID(member_json["ID"].get<int>());
                m.setType(member_json["Type"].get<string>());
                m.setFines(member_json["Fines"].get<int>());

                // Loop through all loans
                for (auto &it: Loan::getLoans_List()) {
                    // If the member has a loan
                    if (m.getID() == it.getMemberID()) {
                        // Add the loan to the checked out books
                        m.getCheckedOutBooks().push_back(it);
                    }
                }
                // Add the member to the temporary members list
                loadedMembers.push_back(m);
            }
            else{
                Librarian l;

                // Set the name, ID, type, and fines of the librarian
                l.setName(member_json["Name"].get<string>());
                l.setID(member_json["ID"].get<int>());
                l.setType(member_json["Type"].get<string>());
                l.setFines(member_json["Fines"].get<int>());

                // Add the librarian to the temporary librarians list
                loadedLibrarians.push_back(l);
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse users.json: " << e.what() << std::endl;
        return false;
    }

    Member::getMembers() = loadedMembers;
    LibrarySystem::getInstance().getLibrarians() = loadedLibrarians;
    file.close();
    return true;
}

// Function to save members
bool Librarian::saveMembers() {
    std::ofstream file(resolveDataFilePath("users.json"));
    // If the file is not open
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << endl;
        return false;
    }
    json OUTPUT;
    // Update the total fines for each member
    for(auto& member : Member::getMembers()){
        member.updateTotalFines();
    }
    // Loop through all members
    for(const auto& member : Member::getMembers())
    {
        json bookJson;

        // Save the name, ID, type, and fines of the member
        bookJson["Name"] = member.getName().getSTR();
        bookJson["ID"] = member.getID();
        bookJson["Type"] = member.getType().getSTR();
        bookJson["Fines"] = member.getFines();

        // Add the member to the output
        OUTPUT.push_back(bookJson);
    }
    // Loop through all librarians
    for(const auto& librarian : LibrarySystem::getInstance().getLibrarians())
    {
        json bookJson;

        // Save the name, ID, type, and fines of the librarian

        bookJson["Name"] = librarian.getName().getSTR();
        bookJson["ID"] = librarian.getID();
        bookJson["Type"] = librarian.getType().getSTR();
        bookJson["Fines"] = librarian.getFines();

        OUTPUT.push_back(bookJson);
    }
    file<<setw(4)<<OUTPUT<<endl;
    file.close();
    return true;
}

vector<Librarian>& Librarian::getLibrarians(){
    return LibrarySystem::getInstance().getLibrarians();
}

vector<Loan>& Librarian::getBorrowRequests(){
    return LibrarySystem::getInstance().getBorrowRequests();
}

// Returns "Librarian" — fulfills LibraryEntity contract
Custom_String_Class Librarian::getRole() const {
    return Custom_String_Class("Librarian");
}

int Librarian::getBorrowLimit() const {
    return 0;
}
