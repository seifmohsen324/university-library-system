#ifdef _WIN32
#  include <Windows.h>
#  include <mmsystem.h>
#endif

// Include necessary libraries and files
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "json.h"
#include "Book.h"
#include "User.h"
#include "Loan.h"
#include "Custom_String_Class.h"
#include "Functionalities.h"
#include <thread>
#include "LibrarySystem.h"

using namespace std;
using json = nlohmann::json;

// Get the standard output handle
HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

// Function declarations
void displayLibraryName();
void displayCenteredText(const std::string& text);
void displayCenteredText(const Custom_String_Class& text);
void printDivider(char fill = '=', int width = 72);
void printMenuHeader(const std::string& title);
void printMenuOption(int number, const std::string& text);
void playSound(const char* audioPath);

// Main function
int main() {
    SetConsoleTitleA("The Forgotten Shelf");

    // Path to the sound file
    const std::string audioPath = "music.mp3";

    // Start a new thread to play the sound (if file exists)
    std::thread soundThread([audioPath]() {
        std::ifstream file(audioPath);
        if (file.good()) {
            playSound(audioPath.c_str());
            return;
        }

        const std::string buildAudioPath = "cmake-build-debug\\music.mp3";
        std::ifstream buildFile(buildAudioPath);
        if (buildFile.good()) {
            playSound(buildAudioPath.c_str());
        }
    });

    // Set the console size
    cout << "\x1b[8;200;150t";

    bool running = true;
    while (running) {
        // Display the library name
        displayLibraryName();
        cout.flush();
        Sleep(500);

        // Load the files with error handling
        cout << "\nLoading data files..." << endl;
        cout.flush();
        try {
            loadFiles();
            cout << "Data files loaded successfully!" << endl;
            cout.flush();
        } catch (const std::exception& e) {
            cerr << "Error loading files: " << e.what() << endl;
            if (soundThread.joinable()) {
                soundThread.join();
            }
            return 1;
        }

        // Set the console text attribute to white/visible
        SetConsoleTextAttribute(hConsole, 7);

        // Prompt the user to enter their ID
        cout << endl << "Enter your ID: ";
        cout.flush();
        int id;
        if (!readInt(id)) {
            cout << "Invalid input. Please enter a numeric ID." << endl;
            if (soundThread.joinable()) {
                soundThread.join();
            }
            return 1;
        }

        // Clear the console
        system("cls");

        // Login with the entered ID
        Custom_String_Class type = login(id);

        // Set the console text attribute
        SetConsoleTextAttribute(hConsole, 3);

        if (type == "member") {
            Member* memberPtr = nullptr;
            for (auto& m : Member::getMembers()) {
                if (m.getID() == id) {
                    memberPtr = &m;
                    break;
                }
            }
            if (!memberPtr) {
                cout << "Could not locate your member record." << endl;
                if (soundThread.joinable()) {
                    soundThread.join();
                }
                return 1;
            }

            bool sessionActive = true;
            while (sessionActive) {
                displayLibraryName();
                displayCenteredText(Custom_String_Class("Welcome ") + memberPtr->getName() + Custom_String_Class(" to our library"));
                displayMenuMember();

                int memberOption;
                if (!readInt(memberOption)) {
                    cout << "Invalid input. Please enter a numeric option." << endl;
                    continue;
                }

                if (memberOption == 7) {
                    saveFiles();
                    system("cls");
                    break;
                }
                if (memberOption == 8) {
                    sessionActive = false;
                    running = false;
                    break;
                }
                if (memberOption < 1 || memberOption > 8) {
                    cout << "Invalid choice" << endl;
                    continue;
                }

                system("cls");
                implementMemberChoice(*memberPtr, memberOption);
                system("pause");
                system("cls");
            }
        } else if (type == "librarian") {
            Librarian* librarianPtr = nullptr;
            for (auto& l : Librarian::getLibrarians()) {
                if (l.getID() == id) {
                    librarianPtr = &l;
                    break;
                }
            }
            if (!librarianPtr) {
                cout << "Could not locate your librarian record." << endl;
                if (soundThread.joinable()) {
                    soundThread.join();
                }
                return 1;
            }

            bool sessionActive = true;
            while (sessionActive) {
                displayLibraryName();
                displayCenteredText(Custom_String_Class("Welcome back ") + librarianPtr->getName());
                displayMenuLibrarian();

                int librarianOption;
                if (!readInt(librarianOption)) {
                    cout << "Invalid input. Please enter a numeric option." << endl;
                    continue;
                }

                if (librarianOption == 15) {
                    saveFiles();
                    system("cls");
                    break;
                }
                if (librarianOption == 16) {
                    sessionActive = false;
                    running = false;
                    break;
                }
                if (librarianOption < 1 || librarianOption > 16) {
                    cout << "Invalid choice" << endl;
                    continue;
                }

                system("cls");
                implementLibrarianChoice(*librarianPtr, librarianOption);
                system("pause");
                system("cls");
            }
        } else if (type == "not found") {
            cout << "You are not registered in our system, Please check your ID again!!" << endl << endl;
            continue;
        }
    }

    // Save the files
    saveFiles();

    // Pause the console
    system("pause");

    // Join the sound thread
    soundThread.join();

    return 0;
}

// Function to display the library name
void displayLibraryName(){
    SetConsoleTextAttribute(hConsole, 6);
    cout << R"(
  ___ _   _ ___   ___ ___  ___ ___  ___ _____ _____ ___ _  _ 
 |_ _| |_| | __| | __/ _ \| _ \ __ / _ |_   _|_   _| __| \| |
  | ||  _  | _|  | _| (_) |   / _ | (_) || |   | | | _|| . `|
  |_||_| |_|___| |_| \___/|_|_\___|\___/ |_|   |_| |___|_|\_|
                ___ _  _ ___ _    ___ 
               / __| || | __| |  | __|
               \__ \  _  | _| |__| _| 
               |___/_| |_|___|____|_| 
)";
    printDivider('=');
}

void printDivider(char fill, int width) {
    cout << std::string(width, fill) << endl;
}

void printMenuHeader(const std::string& title) {
    printDivider('=');
    displayCenteredText(title);
    printDivider('=');
}

void printMenuOption(int number, const std::string& text) {
    cout << "  " << setw(2) << number << ". " << text << '\n';
}

// Function to center a single line of text in the console
void displayCenteredText(const std::string& text) {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo{};
    int consoleWidth = 120;
    if (GetConsoleScreenBufferInfo(hConsole, &consoleInfo)) {
        consoleWidth = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    }

    int padding = (consoleWidth - static_cast<int>(text.size())) / 2;
    if (padding < 0) {
        padding = 0;
    }

    cout << "\n" << std::string(padding, ' ') << text << endl;
}

// Function to center custom string text in the console
void displayCenteredText(const Custom_String_Class& text) {
    displayCenteredText(text.toString());
}

// Function to display the member menu
void displayMenuMember(){
    SetConsoleTextAttribute(hConsole, 3);
    printMenuHeader("Member Menu");
    printMenuOption(1, "Request to borrow a book");
    printMenuOption(2, "Return a book");
    printMenuOption(3, "Display loaned books");
    printMenuOption(4, "Display your information");
    printMenuOption(5, "Display book list");
    printMenuOption(6, "Find a book");
    printMenuOption(7, "Logout");
    printMenuOption(8, "Exit");
    printDivider('-');
}

// Function to display the librarian menu
void displayMenuLibrarian()
{
    SetConsoleTextAttribute(hConsole, 3);
    printMenuHeader("Librarian Menu");
    printMenuOption(1, "Add book");
    printMenuOption(2, "Remove book");
    printMenuOption(3, "Register new member");
    printMenuOption(4, "Remove member");
    printMenuOption(5, "Edit book");
    printMenuOption(6, "Approve borrow request");
    printMenuOption(7, "Return book");
    printMenuOption(8, "Display all users");
    printMenuOption(9, "Display all books");
    printMenuOption(10, "Display borrow requests");
    printMenuOption(11, "Display all loans");
    printMenuOption(12, "Find a book");
    printMenuOption(13, "Find a member");
    printMenuOption(14, "Find a librarian");
    printMenuOption(15, "Logout");
    printMenuOption(16, "Exit");
    printDivider('-');
}

// Function to display the find book menu
void displayMenuFindBook(){
    SetConsoleTextAttribute(hConsole, 3);
    printMenuHeader("Find Book");
    printMenuOption(1, "Find the book by ISBN");
    printMenuOption(2, "Find the book by name");
    printMenuOption(3, "Find the book by author");
    printDivider('-');
}

// Function to display the find member menu
void displayMenuFindMember(){
    SetConsoleTextAttribute(hConsole, 3);
    printMenuHeader("Find Member");
    printMenuOption(1, "Find the member by name");
    printMenuOption(2, "Find the member by ID");
    printDivider('-');
}

// Function to display the find librarian menu
void displayMenuFindlibrarian(){
    SetConsoleTextAttribute(hConsole, 3);
    printMenuHeader("Find Librarian");
    printMenuOption(1, "Find the librarian by name");
    printMenuOption(2, "Find the librarian by ID");
    printDivider('-');
}

// Function to play a sound
void playSound(const char* audioPath) {
    std::string closeCommand = "close library_music";
    mciSendStringA(closeCommand.c_str(), NULL, 0, NULL);

    std::string openCommand = std::string("open \"") + audioPath + "\" type mpegvideo alias library_music";
    if (mciSendStringA(openCommand.c_str(), NULL, 0, NULL) == 0) {
        mciSendStringA("play library_music repeat", NULL, 0, NULL);
    }
}
