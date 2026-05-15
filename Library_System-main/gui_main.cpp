#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include "Book.h"
#include "Functionalities.h"
#include "LibrarySystem.h"
#include "User.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

void displayMenuFindBook() {}
void displayMenuFindMember() {}
void displayMenuFindlibrarian() {}

namespace {

constexpr int ID_LOGIN_INPUT = 100;
constexpr int ID_LOGIN_BUTTON = 101;
constexpr int ID_LOGOUT_BUTTON = 102;
constexpr int ID_LIST = 110;
constexpr int ID_STATUS = 111;
constexpr int ID_HEADER = 112;

constexpr int ID_NAV_1 = 201;
constexpr int ID_NAV_2 = 202;
constexpr int ID_NAV_3 = 203;
constexpr int ID_NAV_4 = 204;
constexpr int ID_NAV_5 = 205;

constexpr int ID_BOOK_TITLE = 301;
constexpr int ID_BOOK_AUTHOR = 302;
constexpr int ID_BOOK_ISBN = 303;
constexpr int ID_BOOK_GENRE = 304;
constexpr int ID_BOOK_YEAR = 305;
constexpr int ID_BOOK_QTY = 306;
constexpr int ID_BOOK_SEARCH = 307;
constexpr int ID_BOOK_ADD = 308;
constexpr int ID_BOOK_REMOVE = 309;
constexpr int ID_BOOK_REQUEST = 310;
constexpr int ID_BOOK_SEARCH_BTN = 311;
constexpr int ID_BOOK_EDIT = 312;

constexpr int ID_MEMBER_NAME = 401;
constexpr int ID_MEMBER_ID = 402;
constexpr int ID_MEMBER_TYPE = 403;
constexpr int ID_MEMBER_ADD = 404;
constexpr int ID_MEMBER_REMOVE = 405;
constexpr int ID_MEMBER_SEARCH = 406;
constexpr int ID_MEMBER_SEARCH_BTN = 407;

constexpr int ID_RETURN_LOAN = 501;

constexpr int ID_REQUEST_MEMBER_ID = 601;
constexpr int ID_REQUEST_BOOK_ISBN = 602;
constexpr int ID_REQUEST_CREATE = 603;
constexpr int ID_REQUEST_APPROVE = 604;

constexpr int ID_NAV_6 = 206;
constexpr int ID_USER_SEARCH = 701;
constexpr int ID_USER_SEARCH_BTN = 702;

enum class Role { None, Member, Librarian };
enum class View { Login, Dashboard, Books, Members, Loans, Requests, Profile, Users };

HINSTANCE g_instance{};
HWND g_main{};
HWND g_loginPanel{};
HWND g_navPanel{};
HWND g_list{};
HWND g_status{};
HWND g_header{};
HWND g_loginStatus{};
HWND g_booksPanel{};
HWND g_membersPanel{};
HWND g_loansPanel{};
HWND g_requestsPanel{};
HWND g_usersPanel{};
HFONT g_titleFont{};
HFONT g_headerFont{};
HFONT g_uiFont{};
HBRUSH g_windowBrush{};
HBRUSH g_panelBrush{};
HBRUSH g_statusBrush{};
HBRUSH g_loginBrush{};
HBRUSH g_navBrush{};
HBRUSH g_listBrush{};
COLORREF g_textColor = RGB(59, 42, 31);
COLORREF g_accentColor = RGB(128, 45, 45);
COLORREF g_windowColor = RGB(244, 236, 216);
COLORREF g_panelColor = RGB(226, 232, 206);
COLORREF g_loginColor = RGB(239, 224, 196);
COLORREF g_navColor = RGB(82, 103, 73);
COLORREF g_statusColor = RGB(250, 231, 174);
COLORREF g_listColor = RGB(255, 252, 242);

Role g_role = Role::None;
View g_view = View::Login;
int g_currentId = 0;

std::vector<size_t> g_bookRows;
std::vector<size_t> g_memberRows;
std::vector<size_t> g_loanRows;
std::vector<size_t> g_requestRows;

HWND addControl(HWND parent, const char* klass, const char* text, DWORD style,
                int x, int y, int w, int h, int id = 0) {
    HWND control = CreateWindowExA(0, klass, text, WS_CHILD | WS_VISIBLE | style,
                                   x, y, w, h, parent, reinterpret_cast<HMENU>(id),
                                   g_instance, nullptr);
    if (g_uiFont) {
        SendMessageA(control, WM_SETFONT, reinterpret_cast<WPARAM>(g_uiFont), TRUE);
    }
    return control;
}

void addLabeledEdit(HWND parent, const char* label, int id, int x, int y, int w) {
    addControl(parent, "STATIC", label, 0, x, y + 4, 90, 22);
    addControl(parent, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL, x + 95, y, w, 24, id);
}

std::string getText(HWND parent, int id) {
    char buffer[512]{};
    GetWindowTextA(GetDlgItem(parent, id), buffer, static_cast<int>(sizeof(buffer)));
    return buffer;
}

void setText(HWND parent, int id, const std::string& text) {
    SetWindowTextA(GetDlgItem(parent, id), text.c_str());
}

int parseInt(const std::string& text, bool& ok) {
    try {
        size_t pos = 0;
        int value = std::stoi(text, &pos);
        ok = pos == text.size();
        return value;
    } catch (...) {
        ok = false;
        return 0;
    }
}

std::string lower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return text;
}

bool containsText(const Custom_String_Class& value, const std::string& needle) {
    return lower(value.toString()).find(lower(needle)) != std::string::npos;
}

bool containsText(const std::string& value, const std::string& needle) {
    return lower(value).find(lower(needle)) != std::string::npos;
}

std::string maskId(int id) {
    std::string text = std::to_string(id);
    if (text.size() <= 2) {
        return std::string(text.size(), '*');
    }
    return std::string(text.size() - 2, '*') + text.substr(text.size() - 2);
}

std::string visibleId(int id) {
    return g_role == Role::Librarian ? std::to_string(id) : maskId(id);
}

void setStatus(const std::string& message) {
    SetWindowTextA(g_status, message.c_str());
}

void setLoginStatus(const std::string& message) {
    SetWindowTextA(g_loginStatus, message.c_str());
}

void setHeader(const std::string& title) {
    SetWindowTextA(g_header, ("The Forgotten Shelf - " + title).c_str());
}

LRESULT CALLBACK panelProc(HWND panel, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_COMMAND) {
        SendMessageA(g_main, message, wParam, lParam);
        return 0;
    }
    if (message == WM_CTLCOLORSTATIC) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        const bool loginPanel = panel == g_loginPanel;
        SetTextColor(dc, g_textColor);
        SetBkColor(dc, loginPanel ? g_loginColor : g_panelColor);
        return reinterpret_cast<LRESULT>(loginPanel ? g_loginBrush : g_panelBrush);
    }
    if (message == WM_CTLCOLOREDIT) {
        HDC dc = reinterpret_cast<HDC>(wParam);
        SetTextColor(dc, g_textColor);
        SetBkColor(dc, g_listColor);
        return reinterpret_cast<LRESULT>(g_listBrush);
    }
    return DefWindowProcA(panel, message, wParam, lParam);
}

void forwardPanelCommands(HWND panel) {
    SetWindowLongPtrA(panel, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(panelProc));
}

void clearList() {
    SendMessageA(g_list, LB_RESETCONTENT, 0, 0);
}

void addLine(const std::string& line) {
    SendMessageA(g_list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(line.c_str()));
}

int selectedRow() {
    return static_cast<int>(SendMessageA(g_list, LB_GETCURSEL, 0, 0));
}

Member* currentMember() {
    for (auto& member : Member::getMembers()) {
        if (member.getID() == g_currentId) return &member;
    }
    return nullptr;
}

Librarian* currentLibrarian() {
    for (auto& librarian : Librarian::getLibrarians()) {
        if (librarian.getID() == g_currentId) return &librarian;
    }
    return nullptr;
}

Member* findMember(int id) {
    for (auto& member : Member::getMembers()) {
        if (member.getID() == id) return &member;
    }
    return nullptr;
}

Book* findBook(const Custom_String_Class& isbn) {
    for (auto& book : Book::getBookList()) {
        if (book.getISBN() == isbn) return &book;
    }
    return nullptr;
}

bool idExists(int id) {
    if (findMember(id)) return true;
    for (const auto& librarian : Librarian::getLibrarians()) {
        if (librarian.getID() == id) return true;
    }
    return false;
}

std::string bookLine(Book& book, size_t index) {
    std::ostringstream out;
    out << index + 1 << ". " << book.getTitle().toString()
        << " | ISBN: " << book.getISBN().toString()
        << " | " << book.getAuthor().toString()
        << " | " << book.getGenre().toString()
        << " | Qty " << book.getQuantity()
        << " | " << (book.isRare() ? "Rare" : "Regular");
    return out.str();
}

std::string memberLine(Member& member, size_t index) {
    std::ostringstream out;
    out << index + 1 << ". " << member.getName().toString()
        << " | Access " << visibleId(member.getID())
        << " | " << member.getType().toString()
        << " | Limit " << member.getBorrowLimit()
        << " | Active loans " << member.getCheckedOutBooks().size()
        << " | Fines " << member.calculateTotalFines();
    return out.str();
}

std::string librarianLine(const Librarian& librarian, size_t index) {
    std::ostringstream out;
    out << index + 1 << ". " << librarian.getName().toString()
        << " | Access " << visibleId(librarian.getID())
        << " | Librarian";
    return out.str();
}

std::string loanLine(const Loan& loan, size_t index) {
    std::ostringstream out;
    out << index + 1 << ". Member " << visibleId(loan.getMemberID())
        << " | ISBN " << loan.getBookID().toString()
        << " | Borrowed " << loan.getBorrowDate().getDate().toString()
        << " | Due " << loan.getDueDate().getDate().toString();
    return out.str();
}

std::string requestLine(const Loan& request, size_t index) {
    std::ostringstream out;
    out << index + 1 << ". Member " << visibleId(request.getMemberID())
        << " | ISBN " << request.getBookID().toString();
    return out.str();
}

size_t removeRequestsForBook(const Custom_String_Class& isbn) {
    auto& requests = Librarian::getBorrowRequests();
    const size_t before = requests.size();
    requests.erase(std::remove_if(requests.begin(), requests.end(),
        [&](const Loan& request) { return request.getBookID() == isbn; }), requests.end());
    return before - requests.size();
}

size_t removeRequestsForMember(int memberId) {
    auto& requests = Librarian::getBorrowRequests();
    const size_t before = requests.size();
    requests.erase(std::remove_if(requests.begin(), requests.end(),
        [&](const Loan& request) { return request.getMemberID() == memberId; }), requests.end());
    return before - requests.size();
}

void showPanel(HWND panel, bool visible) {
    ShowWindow(panel, visible ? SW_SHOW : SW_HIDE);
}

void showApp(bool visible) {
    const int mode = visible ? SW_SHOW : SW_HIDE;
    ShowWindow(g_navPanel, mode);
    ShowWindow(g_list, mode);
    ShowWindow(g_status, mode);
    ShowWindow(g_header, mode);
}

void hideSidePanels() {
    showPanel(g_booksPanel, false);
    showPanel(g_membersPanel, false);
    showPanel(g_loansPanel, false);
    showPanel(g_requestsPanel, false);
    showPanel(g_usersPanel, false);
}

void refreshDashboard() {
    hideSidePanels();
    clearList();
    setHeader("Dashboard");

    addLine("The Forgotten Shelf - Library System");
    addLine("------------------------------------------------------------");
    addLine("Books: " + std::to_string(Book::getBookList().size()));
    addLine("Members: " + std::to_string(Member::getMembers().size()));
    addLine("Librarians: " + std::to_string(Librarian::getLibrarians().size()));
    addLine("Active loans: " + std::to_string(Loan::getLoans_List().size()));
    addLine("Pending borrow requests: " + std::to_string(Librarian::getBorrowRequests().size()));

    if (g_role == Role::Member) {
        Member* member = currentMember();
        if (member) {
            addLine("------------------------------------------------------------");
            addLine("Signed in as " + member->getName().toString() + " (" + member->getType().toString() + ")");
            addLine("Borrow limit: " + std::to_string(member->getBorrowLimit()));
            addLine("Current fines: " + std::to_string(member->calculateTotalFines()));
            setStatus("Signed in as member: " + member->getName().toString());
            return;
        }
        setStatus("Member profile not found.");
        return;
    }

    if (g_role == Role::Librarian) {
        Librarian* librarian = currentLibrarian();
        if (librarian) {
            addLine("------------------------------------------------------------");
            addLine("Signed in as " + librarian->getName().toString() + " (Librarian)");
            setStatus("Signed in as librarian: " + librarian->getName().toString());
            return;
        }
        setStatus("Librarian profile not found.");
        return;
    }

    setStatus("Not signed in.");
}

void refreshBooks() {
    clearList();
    g_bookRows.clear();
    setHeader(g_role == Role::Librarian ? "Admin - Book Catalog" : "Member - Book Catalog");
    showPanel(g_booksPanel, true);
    showPanel(g_membersPanel, false);
    showPanel(g_loansPanel, false);
    showPanel(g_requestsPanel, false);
    showPanel(g_usersPanel, false);

    ShowWindow(GetDlgItem(g_booksPanel, ID_BOOK_ADD), g_role == Role::Librarian ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_booksPanel, ID_BOOK_REMOVE), g_role == Role::Librarian ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_booksPanel, ID_BOOK_EDIT), g_role == Role::Librarian ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_booksPanel, ID_BOOK_REQUEST), g_role == Role::Member ? SW_SHOW : SW_HIDE);

    const std::string search = getText(g_booksPanel, ID_BOOK_SEARCH);
    auto& books = Book::getBookList();
    for (size_t i = 0; i < books.size(); ++i) {
        const bool matches = search.empty()
            || containsText(books[i].getTitle(), search)
            || containsText(books[i].getAuthor(), search)
            || containsText(books[i].getISBN(), search)
            || containsText(books[i].getGenre(), search);
        if (matches) {
            g_bookRows.push_back(i);
            addLine(bookLine(books[i], i));
        }
    }

    setStatus(std::to_string(g_bookRows.size()) + " books shown.");
}

void refreshMembers() {
    clearList();
    g_memberRows.clear();
    setHeader("Admin - Members");
    showPanel(g_booksPanel, false);
    showPanel(g_membersPanel, true);
    showPanel(g_loansPanel, false);
    showPanel(g_requestsPanel, false);
    showPanel(g_usersPanel, false);

    const std::string search = getText(g_membersPanel, ID_MEMBER_SEARCH);
    auto& members = Member::getMembers();
    for (size_t i = 0; i < members.size(); ++i) {
        const bool matches = search.empty()
            || containsText(members[i].getName(), search)
            || containsText(members[i].getType(), search)
            || containsText(std::to_string(members[i].getID()), search);
        if (matches) {
            g_memberRows.push_back(i);
            addLine(memberLine(members[i], i));
        }
    }

    setStatus(std::to_string(g_memberRows.size()) + " members shown.");
}

void refreshLoans() {
    clearList();
    g_loanRows.clear();
    setHeader(g_role == Role::Librarian ? "Admin - Active Loans" : "My Loans");
    showPanel(g_booksPanel, false);
    showPanel(g_membersPanel, false);
    showPanel(g_loansPanel, true);
    showPanel(g_requestsPanel, false);
    showPanel(g_usersPanel, false);

    auto& loans = Loan::getLoans_List();
    for (size_t i = 0; i < loans.size(); ++i) {
        if (g_role == Role::Member && loans[i].getMemberID() != g_currentId) {
            continue;
        }
        g_loanRows.push_back(i);
        addLine(loanLine(loans[i], i));
    }

    setStatus(std::to_string(g_loanRows.size()) + " loans shown.");
}

void refreshRequests() {
    clearList();
    g_requestRows.clear();
    setHeader("Admin - Borrow Requests");
    showPanel(g_booksPanel, false);
    showPanel(g_membersPanel, false);
    showPanel(g_loansPanel, false);
    showPanel(g_requestsPanel, true);
    showPanel(g_usersPanel, false);

    auto& requests = Librarian::getBorrowRequests();
    for (size_t i = 0; i < requests.size(); ++i) {
        g_requestRows.push_back(i);
        addLine(requestLine(requests[i], i));
    }

    setStatus(std::to_string(requests.size()) + " pending requests.");
}

void refreshUsers() {
    clearList();
    setHeader("Admin - All Users");
    showPanel(g_booksPanel, false);
    showPanel(g_membersPanel, false);
    showPanel(g_loansPanel, false);
    showPanel(g_requestsPanel, false);
    showPanel(g_usersPanel, true);

    const std::string search = getText(g_usersPanel, ID_USER_SEARCH);
    size_t shown = 0;
    addLine("Members");
    addLine("------------------------------------------------------------");
    for (size_t i = 0; i < Member::getMembers().size(); ++i) {
        auto& member = Member::getMembers()[i];
        const bool matches = search.empty()
            || containsText(member.getName(), search)
            || containsText(member.getType(), search)
            || containsText(std::to_string(member.getID()), search);
        if (matches) {
            addLine(memberLine(member, i));
            ++shown;
        }
    }
    addLine("");
    addLine("Librarians");
    addLine("------------------------------------------------------------");
    for (size_t i = 0; i < Librarian::getLibrarians().size(); ++i) {
        const auto& librarian = Librarian::getLibrarians()[i];
        const bool matches = search.empty()
            || containsText(librarian.getName(), search)
            || containsText(std::to_string(librarian.getID()), search)
            || containsText(std::string("Librarian"), search);
        if (matches) {
            addLine(librarianLine(librarian, i));
            ++shown;
        }
    }

    setStatus(std::to_string(shown) + " users shown.");
}

void refreshProfile() {
    hideSidePanels();
    clearList();
    setHeader("My Profile");

    Member* member = currentMember();
    if (!member) {
        setStatus("Member profile not found.");
        return;
    }

    addLine("Name: " + member->getName().toString());
    addLine("Access ID: " + visibleId(member->getID()));
    addLine("Type: " + member->getType().toString());
    addLine("Borrow limit: " + std::to_string(member->getBorrowLimit()));
    addLine("Active loans: " + std::to_string(member->getCheckedOutBooks().size()));
    addLine("Current fines: " + std::to_string(member->calculateTotalFines()));
    setStatus("Profile loaded.");
}

void refreshCurrent() {
    if (g_view == View::Dashboard) refreshDashboard();
    else if (g_view == View::Books) refreshBooks();
    else if (g_view == View::Members) refreshMembers();
    else if (g_view == View::Loans) refreshLoans();
    else if (g_view == View::Requests) refreshRequests();
    else if (g_view == View::Profile) refreshProfile();
    else if (g_view == View::Users) refreshUsers();
}

void configureNav() {
    if (g_role == Role::Librarian) {
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_1), "Dashboard");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_2), "Books");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_3), "Members");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_4), "Loans");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_5), "Users");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_6), "Requests");
        ShowWindow(GetDlgItem(g_navPanel, ID_NAV_5), SW_SHOW);
        ShowWindow(GetDlgItem(g_navPanel, ID_NAV_6), SW_SHOW);
    } else {
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_1), "Dashboard");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_2), "Catalog");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_3), "My Loans");
        SetWindowTextA(GetDlgItem(g_navPanel, ID_NAV_4), "Profile");
        ShowWindow(GetDlgItem(g_navPanel, ID_NAV_5), SW_HIDE);
        ShowWindow(GetDlgItem(g_navPanel, ID_NAV_6), SW_HIDE);
    }
}

void switchView(View view) {
    g_view = view;
    refreshCurrent();
}

void login() {
    bool ok = false;
    const int id = parseInt(getText(g_loginPanel, ID_LOGIN_INPUT), ok);
    if (!ok) {
        setLoginStatus("Enter a valid numeric access ID.");
        return;
    }

    for (const auto& librarian : Librarian::getLibrarians()) {
        if (librarian.getID() == id) {
            g_role = Role::Librarian;
            g_currentId = id;
            ShowWindow(g_loginPanel, SW_HIDE);
            showApp(true);
            configureNav();
            switchView(View::Dashboard);
            setStatus("Signed in as librarian: " + librarian.getName().toString());
            setLoginStatus("");
            return;
        }
    }

    for (const auto& member : Member::getMembers()) {
        if (member.getID() == id) {
            g_role = Role::Member;
            g_currentId = id;
            ShowWindow(g_loginPanel, SW_HIDE);
            showApp(true);
            configureNav();
            switchView(View::Dashboard);
            setStatus("Signed in as member: " + member.getName().toString());
            setLoginStatus("");
            return;
        }
    }

    setLoginStatus("Access ID was not found.");
}

void logout() {
    saveFiles();
    g_role = Role::None;
    g_currentId = 0;
    g_view = View::Login;
    clearList();
    hideSidePanels();
    showApp(false);
    setLoginStatus("");
    setText(g_loginPanel, ID_LOGIN_INPUT, "");
    ShowWindow(g_loginPanel, SW_SHOW);
}

void addBookFromForm() {
    const std::string title = getText(g_booksPanel, ID_BOOK_TITLE);
    const std::string author = getText(g_booksPanel, ID_BOOK_AUTHOR);
    const std::string isbn = getText(g_booksPanel, ID_BOOK_ISBN);
    const std::string genre = getText(g_booksPanel, ID_BOOK_GENRE);
    bool yearOk = false;
    bool qtyOk = false;
    int year = parseInt(getText(g_booksPanel, ID_BOOK_YEAR), yearOk);
    int quantity = parseInt(getText(g_booksPanel, ID_BOOK_QTY), qtyOk);

    if (title.empty() || author.empty() || isbn.empty() || genre.empty()) {
        setStatus("Book fields cannot be empty.");
        return;
    }
    if (!yearOk || year < 1000 || year > 2100 || !qtyOk || quantity <= 0) {
        setStatus("Enter a valid year and positive quantity.");
        return;
    }
    if (findBook(Custom_String_Class(isbn))) {
        setStatus("A book with this ISBN already exists.");
        return;
    }

    Book::getBookList().push_back(Book(title, author, isbn, genre, year, quantity));
    saveFiles();
    refreshBooks();
    setStatus("Book added and saved.");
}

void fillBookFormFromSelection() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_bookRows.size())) {
        setStatus("Select a book first.");
        return;
    }

    Book& book = Book::getBookList()[g_bookRows[row]];
    setText(g_booksPanel, ID_BOOK_TITLE, book.getTitle().toString());
    setText(g_booksPanel, ID_BOOK_AUTHOR, book.getAuthor().toString());
    setText(g_booksPanel, ID_BOOK_ISBN, book.getISBN().toString());
    setText(g_booksPanel, ID_BOOK_GENRE, book.getGenre().toString());
    setText(g_booksPanel, ID_BOOK_YEAR, std::to_string(book.getPublicationYear()));
    setText(g_booksPanel, ID_BOOK_QTY, std::to_string(book.getQuantity()));
    setStatus("Book loaded into the edit form.");
}

void editSelectedBookFromForm() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_bookRows.size())) {
        setStatus("Select a book first.");
        return;
    }

    const std::string title = getText(g_booksPanel, ID_BOOK_TITLE);
    const std::string author = getText(g_booksPanel, ID_BOOK_AUTHOR);
    const std::string isbn = getText(g_booksPanel, ID_BOOK_ISBN);
    const std::string genre = getText(g_booksPanel, ID_BOOK_GENRE);
    bool yearOk = false;
    bool qtyOk = false;
    const int year = parseInt(getText(g_booksPanel, ID_BOOK_YEAR), yearOk);
    const int quantity = parseInt(getText(g_booksPanel, ID_BOOK_QTY), qtyOk);

    if (title.empty() || author.empty() || isbn.empty() || genre.empty()) {
        setStatus("Book fields cannot be empty.");
        return;
    }
    if (!yearOk || year < 1000 || year > 2100 || !qtyOk || quantity < 0) {
        setStatus("Enter a valid year and non-negative quantity.");
        return;
    }

    auto& books = Book::getBookList();
    Book& book = books[g_bookRows[row]];
    const Custom_String_Class oldIsbn = book.getISBN();
    const Custom_String_Class newIsbn(isbn);
    if (!(oldIsbn == newIsbn)) {
        for (const auto& loan : Loan::getLoans_List()) {
            if (loan.getBookID() == oldIsbn) {
                setStatus("Cannot change ISBN while this book is loaned.");
                return;
            }
        }
        for (size_t i = 0; i < books.size(); ++i) {
            if (i != g_bookRows[row] && books[i].getISBN() == newIsbn) {
                setStatus("A book with this ISBN already exists.");
                return;
            }
        }
    }

    book.getTitle() = Custom_String_Class(title);
    book.getAuthor() = Custom_String_Class(author);
    book.setISBN(newIsbn);
    book.getGenre() = Custom_String_Class(genre);
    book.setPubYear(static_cast<unsigned int>(year));
    book.setQuantity(static_cast<unsigned int>(quantity));

    size_t removedRequests = 0;
    if (!(oldIsbn == newIsbn)) {
        removedRequests = removeRequestsForBook(oldIsbn);
    }

    saveFiles();
    refreshBooks();
    setStatus(std::string("Book updated and saved.") +
              (removedRequests ? " Removed stale requests for the old ISBN." : ""));
}

void removeSelectedBook() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_bookRows.size())) {
        setStatus("Select a book first.");
        return;
    }

    auto& books = Book::getBookList();
    const size_t index = g_bookRows[row];
    const Custom_String_Class isbn = books[index].getISBN();
    for (const auto& loan : Loan::getLoans_List()) {
        if (loan.getBookID() == isbn) {
            setStatus("Cannot remove a book that is currently loaned.");
            return;
        }
    }

    if (MessageBoxA(g_main, "Remove the selected book from the catalog?", "Confirm remove",
                    MB_YESNO | MB_ICONQUESTION) != IDYES) {
        setStatus("Book removal cancelled.");
        return;
    }

    const size_t removedRequests = removeRequestsForBook(isbn);
    books.erase(books.begin() + static_cast<long long>(index));
    saveFiles();
    refreshBooks();
    setStatus(std::string("Book removed and saved.") +
              (removedRequests ? " Pending requests for it were removed." : ""));
}

void requestSelectedBook() {
    const int row = selectedRow();
    Member* member = currentMember();
    if (!member || row < 0 || row >= static_cast<int>(g_bookRows.size())) {
        setStatus("Select a book first.");
        return;
    }

    Book& book = Book::getBookList()[g_bookRows[row]];
    const size_t before = Librarian::getBorrowRequests().size();
    member->requestBorrow(book);
    saveFiles();
    refreshBooks();
    setStatus(Librarian::getBorrowRequests().size() > before
        ? "Borrow request submitted."
        : "Request was not created. Check availability, limit, or duplicates.");
}

void addMemberFromForm() {
    const std::string name = getText(g_membersPanel, ID_MEMBER_NAME);
    std::string type = lower(getText(g_membersPanel, ID_MEMBER_TYPE));
    bool idOk = false;
    const int id = parseInt(getText(g_membersPanel, ID_MEMBER_ID), idOk);

    if (name.empty() || type.empty() || !idOk) {
        setStatus("Enter member name, numeric ID, and type.");
        return;
    }
    if (type != "member" && type != "student" && type != "staff" && type != "faculty") {
        setStatus("Type must be Member, Student, Staff, or Faculty.");
        return;
    }
    if (idExists(id)) {
        setStatus("This access ID already exists.");
        return;
    }

    type[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(type[0])));
    Member::getMembers().push_back(Member(name, id, type));
    saveFiles();
    refreshMembers();
    setStatus("Member added and saved.");
}

void removeSelectedMember() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_memberRows.size())) {
        setStatus("Select a member first.");
        return;
    }

    auto& members = Member::getMembers();
    const size_t index = g_memberRows[row];
    const int memberId = members[index].getID();

    for (const auto& loan : Loan::getLoans_List()) {
        if (loan.getMemberID() == memberId) {
            setStatus("Cannot remove a member with active loans.");
            return;
        }
    }

    if (MessageBoxA(g_main, "Remove the selected member?", "Confirm remove",
                    MB_YESNO | MB_ICONQUESTION) != IDYES) {
        setStatus("Member removal cancelled.");
        return;
    }

    const size_t removedRequests = removeRequestsForMember(memberId);
    members.erase(members.begin() + static_cast<long long>(index));
    saveFiles();
    refreshMembers();
    setStatus(std::string("Member removed and saved.") +
              (removedRequests ? " Pending requests for this member were removed." : ""));
}

void returnSelectedLoan() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_loanRows.size())) {
        setStatus("Select a loan first.");
        return;
    }

    Loan loan = Loan::getLoans_List()[g_loanRows[row]];
    Member* member = findMember(loan.getMemberID());
    Book* book = findBook(loan.getBookID());
    if (!member || !book) {
        setStatus("Could not find matching member or book.");
        return;
    }

    if (MessageBoxA(g_main, "Return the selected loan?", "Confirm return",
                    MB_YESNO | MB_ICONQUESTION) != IDYES) {
        setStatus("Return cancelled.");
        return;
    }

    Librarian librarian;
    librarian.returnBook(*member, *book);
    saveFiles();
    refreshLoans();
    setStatus("Loan returned and saved.");
}

void createBorrowRequest() {
    bool idOk = false;
    const int memberId = parseInt(getText(g_requestsPanel, ID_REQUEST_MEMBER_ID), idOk);
    Member* member = idOk ? findMember(memberId) : nullptr;
    Book* book = findBook(Custom_String_Class(getText(g_requestsPanel, ID_REQUEST_BOOK_ISBN)));
    if (!member || !book) {
        setStatus("Enter an existing member access ID and book ISBN.");
        return;
    }

    const size_t before = Librarian::getBorrowRequests().size();
    member->requestBorrow(*book);
    saveFiles();
    refreshRequests();
    setStatus(Librarian::getBorrowRequests().size() > before
        ? "Borrow request created."
        : "Request was not created. Check rules or duplicates.");
}

void approveSelectedRequest() {
    const int row = selectedRow();
    if (row < 0 || row >= static_cast<int>(g_requestRows.size())) {
        setStatus("Select a request first.");
        return;
    }

    Loan request = Librarian::getBorrowRequests()[g_requestRows[row]];
    if (MessageBoxA(g_main, "Approve the selected borrow request?", "Confirm approval",
                    MB_YESNO | MB_ICONQUESTION) != IDYES) {
        setStatus("Approval cancelled.");
        return;
    }

    Librarian librarian;
    const size_t before = Librarian::getBorrowRequests().size();
    librarian.approveBorrowRequest(request);
    saveFiles();
    refreshRequests();
    setStatus(Librarian::getBorrowRequests().size() < before
        ? "Request approved and loan created."
        : "Request could not be approved.");
}

void createLoginPanel(HWND window) {
    g_loginPanel = addControl(window, "STATIC", "", 0, 0, 0, 1000, 620);
    forwardPanelCommands(g_loginPanel);
    HWND title = addControl(g_loginPanel, "STATIC", "The Forgotten Shelf", 0, 330, 112, 390, 38);
    SendMessageA(title, WM_SETFONT, reinterpret_cast<WPARAM>(g_titleFont), TRUE);
    addControl(g_loginPanel, "STATIC", "Library access desk", 0, 425, 158, 190, 24);
    addControl(g_loginPanel, "STATIC", "Access ID", 0, 315, 211, 80, 24);
    addControl(g_loginPanel, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 400, 205, 210, 30, ID_LOGIN_INPUT);
    addControl(g_loginPanel, "BUTTON", "Login", 0, 620, 204, 90, 32, ID_LOGIN_BUTTON);
    g_loginStatus = addControl(g_loginPanel, "STATIC", "", 0, 315, 252, 410, 26);
}

bool fileExists(const std::string& path) {
    return GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES;
}

std::string executableDirectory() {
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string exePath(path);
    const size_t slash = exePath.find_last_of("\\/");
    return slash == std::string::npos ? "." : exePath.substr(0, slash);
}

std::string findMusicPath() {
    const std::vector<std::string> candidates = {
        "music.mp3",
        "cmake-build-debug\\music.mp3",
        executableDirectory() + "\\music.mp3"
    };

    for (const auto& path : candidates) {
        if (fileExists(path)) {
            return path;
        }
    }
    return {};
}

void playBackgroundMusic() {
    const std::string musicPath = findMusicPath();
    if (musicPath.empty()) {
        return;
    }

    mciSendStringA("close library_gui_music", nullptr, 0, nullptr);
    const std::string openCommand = "open \"" + musicPath + "\" type mpegvideo alias library_gui_music";
    if (mciSendStringA(openCommand.c_str(), nullptr, 0, nullptr) == 0) {
        mciSendStringA("play library_gui_music repeat", nullptr, 0, nullptr);
    }
}

void stopBackgroundMusic() {
    mciSendStringA("stop library_gui_music", nullptr, 0, nullptr);
    mciSendStringA("close library_gui_music", nullptr, 0, nullptr);
}

void createNavigation(HWND window) {
    g_navPanel = addControl(window, "STATIC", "", 0, 0, 0, 1000, 50);
    forwardPanelCommands(g_navPanel);
    addControl(g_navPanel, "BUTTON", "Dashboard", 0, 10, 10, 115, 30, ID_NAV_1);
    addControl(g_navPanel, "BUTTON", "Books", 0, 132, 10, 115, 30, ID_NAV_2);
    addControl(g_navPanel, "BUTTON", "Members", 0, 254, 10, 115, 30, ID_NAV_3);
    addControl(g_navPanel, "BUTTON", "Loans", 0, 376, 10, 115, 30, ID_NAV_4);
    addControl(g_navPanel, "BUTTON", "Users", 0, 498, 10, 115, 30, ID_NAV_5);
    addControl(g_navPanel, "BUTTON", "Requests", 0, 620, 10, 115, 30, ID_NAV_6);
    addControl(g_navPanel, "BUTTON", "Logout", 0, 870, 10, 90, 30, ID_LOGOUT_BUTTON);
}

void createSidePanels(HWND window) {
    g_booksPanel = addControl(window, "STATIC", "", 0, 625, 88, 345, 405);
    forwardPanelCommands(g_booksPanel);
    addLabeledEdit(g_booksPanel, "Title", ID_BOOK_TITLE, 10, 12, 220);
    addLabeledEdit(g_booksPanel, "Author", ID_BOOK_AUTHOR, 10, 44, 220);
    addLabeledEdit(g_booksPanel, "ISBN", ID_BOOK_ISBN, 10, 76, 220);
    addLabeledEdit(g_booksPanel, "Genre", ID_BOOK_GENRE, 10, 108, 220);
    addLabeledEdit(g_booksPanel, "Year", ID_BOOK_YEAR, 10, 140, 220);
    addLabeledEdit(g_booksPanel, "Quantity", ID_BOOK_QTY, 10, 172, 220);
    addControl(g_booksPanel, "BUTTON", "Add Book", 0, 105, 212, 105, 30, ID_BOOK_ADD);
    addControl(g_booksPanel, "BUTTON", "Update Selected", 0, 215, 212, 120, 30, ID_BOOK_EDIT);
    addControl(g_booksPanel, "BUTTON", "Remove Selected", 0, 105, 248, 145, 30, ID_BOOK_REMOVE);
    addControl(g_booksPanel, "BUTTON", "Request Selected", 0, 105, 212, 145, 30, ID_BOOK_REQUEST);
    addLabeledEdit(g_booksPanel, "Search", ID_BOOK_SEARCH, 10, 282, 220);
    addControl(g_booksPanel, "BUTTON", "Search / Refresh", 0, 105, 318, 145, 30, ID_BOOK_SEARCH_BTN);

    g_membersPanel = addControl(window, "STATIC", "", 0, 625, 88, 345, 405);
    forwardPanelCommands(g_membersPanel);
    addLabeledEdit(g_membersPanel, "Name", ID_MEMBER_NAME, 10, 12, 220);
    addControl(g_membersPanel, "STATIC", "Access ID", 0, 10, 48, 90, 22);
    addControl(g_membersPanel, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 105, 44, 220, 24, ID_MEMBER_ID);
    addLabeledEdit(g_membersPanel, "Type", ID_MEMBER_TYPE, 10, 76, 220);
    setText(g_membersPanel, ID_MEMBER_TYPE, "Student");
    addControl(g_membersPanel, "STATIC", "Types: Member, Student, Staff, Faculty", 0, 105, 110, 240, 22);
    addControl(g_membersPanel, "BUTTON", "Add Member", 0, 105, 148, 110, 30, ID_MEMBER_ADD);
    addControl(g_membersPanel, "BUTTON", "Remove Selected", 0, 220, 148, 120, 30, ID_MEMBER_REMOVE);
    addLabeledEdit(g_membersPanel, "Search", ID_MEMBER_SEARCH, 10, 202, 220);
    addControl(g_membersPanel, "BUTTON", "Search / Refresh", 0, 105, 238, 145, 30, ID_MEMBER_SEARCH_BTN);

    g_loansPanel = addControl(window, "STATIC", "", 0, 625, 88, 345, 405);
    forwardPanelCommands(g_loansPanel);
    addControl(g_loansPanel, "BUTTON", "Return Selected Loan", 0, 90, 20, 180, 32, ID_RETURN_LOAN);

    g_requestsPanel = addControl(window, "STATIC", "", 0, 625, 88, 345, 405);
    forwardPanelCommands(g_requestsPanel);
    addControl(g_requestsPanel, "STATIC", "Access ID", 0, 10, 16, 90, 22);
    addControl(g_requestsPanel, "EDIT", "", WS_BORDER | ES_AUTOHSCROLL | ES_PASSWORD, 105, 12, 220, 24, ID_REQUEST_MEMBER_ID);
    addLabeledEdit(g_requestsPanel, "Book ISBN", ID_REQUEST_BOOK_ISBN, 10, 44, 220);
    addControl(g_requestsPanel, "BUTTON", "Create Request", 0, 105, 84, 120, 30, ID_REQUEST_CREATE);
    addControl(g_requestsPanel, "BUTTON", "Approve Selected", 0, 105, 124, 140, 30, ID_REQUEST_APPROVE);

    g_usersPanel = addControl(window, "STATIC", "", 0, 625, 88, 345, 405);
    forwardPanelCommands(g_usersPanel);
    addLabeledEdit(g_usersPanel, "Search", ID_USER_SEARCH, 10, 16, 220);
    addControl(g_usersPanel, "BUTTON", "Search / Refresh", 0, 105, 52, 145, 30, ID_USER_SEARCH_BTN);
}

void createApp(HWND window) {
    createNavigation(window);
    g_header = addControl(window, "STATIC", "Dashboard", 0, 15, 58, 550, 24, ID_HEADER);
    SendMessageA(g_header, WM_SETFONT, reinterpret_cast<WPARAM>(g_headerFont), TRUE);
    g_list = addControl(window, "LISTBOX", "", WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
                        15, 88, 595, 405, ID_LIST);
    g_status = addControl(window, "STATIC", "Ready", WS_BORDER, 15, 510, 955, 30, ID_STATUS);
    createSidePanels(window);
    showApp(false);
    hideSidePanels();
}

LRESULT CALLBACK windowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            createLoginPanel(window);
            createApp(window);
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_LOGIN_BUTTON: login(); break;
                case ID_LOGOUT_BUTTON: logout(); break;
                case ID_NAV_1: switchView(View::Dashboard); break;
                case ID_NAV_2: switchView(View::Books); break;
                case ID_NAV_3: switchView(g_role == Role::Librarian ? View::Members : View::Loans); break;
                case ID_NAV_4: switchView(g_role == Role::Librarian ? View::Loans : View::Profile); break;
                case ID_NAV_5: switchView(View::Users); break;
                case ID_NAV_6: switchView(View::Requests); break;
                case ID_BOOK_ADD: addBookFromForm(); break;
                case ID_BOOK_EDIT: editSelectedBookFromForm(); break;
                case ID_BOOK_REMOVE: removeSelectedBook(); break;
                case ID_BOOK_REQUEST: requestSelectedBook(); break;
                case ID_BOOK_SEARCH_BTN: refreshBooks(); break;
                case ID_MEMBER_ADD: addMemberFromForm(); break;
                case ID_MEMBER_REMOVE: removeSelectedMember(); break;
                case ID_MEMBER_SEARCH_BTN: refreshMembers(); break;
                case ID_RETURN_LOAN: returnSelectedLoan(); break;
                case ID_REQUEST_CREATE: createBorrowRequest(); break;
                case ID_REQUEST_APPROVE: approveSelectedRequest(); break;
                case ID_USER_SEARCH_BTN: refreshUsers(); break;
                case ID_LIST:
                    if (HIWORD(wParam) == LBN_DBLCLK && g_view == View::Books && g_role == Role::Librarian) {
                        fillBookFormFromSelection();
                    }
                    break;
            }
            return 0;

        case WM_CLOSE:
            saveFiles();
            DestroyWindow(window);
            return 0;

        case WM_DESTROY:
            stopBackgroundMusic();
            if (g_titleFont) DeleteObject(g_titleFont);
            if (g_headerFont) DeleteObject(g_headerFont);
            if (g_uiFont) DeleteObject(g_uiFont);
            if (g_windowBrush) DeleteObject(g_windowBrush);
            if (g_panelBrush) DeleteObject(g_panelBrush);
            if (g_statusBrush) DeleteObject(g_statusBrush);
            if (g_loginBrush) DeleteObject(g_loginBrush);
            if (g_navBrush) DeleteObject(g_navBrush);
            if (g_listBrush) DeleteObject(g_listBrush);
            PostQuitMessage(0);
            return 0;

        case WM_CTLCOLORSTATIC: {
            HDC dc = reinterpret_cast<HDC>(wParam);
            HWND control = reinterpret_cast<HWND>(lParam);
            if (control == g_header) {
                SetTextColor(dc, g_accentColor);
                SetBkColor(dc, g_windowColor);
                return reinterpret_cast<LRESULT>(g_windowBrush);
            }
            if (control == g_status) {
                SetTextColor(dc, g_textColor);
                SetBkColor(dc, g_statusColor);
                return reinterpret_cast<LRESULT>(g_statusBrush);
            }
            if (control == g_loginPanel) {
                SetTextColor(dc, g_textColor);
                SetBkColor(dc, g_loginColor);
                return reinterpret_cast<LRESULT>(g_loginBrush);
            }
            if (control == g_navPanel) {
                SetTextColor(dc, RGB(255, 252, 242));
                SetBkColor(dc, g_navColor);
                return reinterpret_cast<LRESULT>(g_navBrush);
            }
            SetTextColor(dc, g_textColor);
            SetBkColor(dc, g_panelColor);
            return reinterpret_cast<LRESULT>(g_panelBrush);
        }

        case WM_CTLCOLORLISTBOX: {
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, g_textColor);
            SetBkColor(dc, g_listColor);
            return reinterpret_cast<LRESULT>(g_listBrush);
        }

        case WM_CTLCOLOREDIT: {
            HDC dc = reinterpret_cast<HDC>(wParam);
            SetTextColor(dc, g_textColor);
            SetBkColor(dc, g_listColor);
            return reinterpret_cast<LRESULT>(g_listBrush);
        }
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

} // namespace

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int showCommand) {
    g_instance = instance;
    g_uiFont = CreateFontA(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    g_headerFont = CreateFontA(24, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    g_titleFont = CreateFontA(34, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    g_windowBrush = CreateSolidBrush(g_windowColor);
    g_panelBrush = CreateSolidBrush(g_panelColor);
    g_statusBrush = CreateSolidBrush(g_statusColor);
    g_loginBrush = CreateSolidBrush(g_loginColor);
    g_navBrush = CreateSolidBrush(g_navColor);
    g_listBrush = CreateSolidBrush(g_listColor);

    try {
        loadFiles();
    } catch (const std::exception& ex) {
        MessageBoxA(nullptr, ex.what(), "Failed to load library data", MB_OK | MB_ICONERROR);
    }

    WNDCLASSA wc{};
    wc.lpfnWndProc = windowProc;
    wc.hInstance = instance;
    wc.lpszClassName = "LibraryGuiWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = g_windowBrush;
    RegisterClassA(&wc);

    g_main = CreateWindowExA(0, wc.lpszClassName, "The Forgotten Shelf - Library GUI",
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                             CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600,
                             nullptr, nullptr, instance, nullptr);
    if (!g_main) return 1;

    ShowWindow(g_main, showCommand);
    UpdateWindow(g_main);
    playBackgroundMusic();

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return static_cast<int>(msg.wParam);
}
