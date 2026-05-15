cmake_minimum_required(VERSION 3.26)
project(LIBRARY)

set(CMAKE_CXX_STANDARD 17)

add_executable(LIBRARY main.cpp
        Book.cpp
        Book.h
        Member.cpp
        Custom_String_Class.cpp
        Custom_String_Class.h
        Librarian.cpp
        User.h
        Loan.cpp
        Loan.h
        json.h
        Date.cpp
        Date.h
        Functionalities.h
        Functionalities.cpp
        LibrarySystem.h
        LibrarySystem.cpp
        DataPaths.h
        DataPaths.cpp
)

# Add winmm library
target_link_libraries(LIBRARY PRIVATE winmm)

add_executable(LIBRARY_GUI WIN32 gui_main.cpp
        Book.cpp
        Book.h
        Member.cpp
        Custom_String_Class.cpp
        Custom_String_Class.h
        Librarian.cpp
        User.h
        Loan.cpp
        Loan.h
        json.h
        Date.cpp
        Date.h
        Functionalities.h
        Functionalities.cpp
        LibrarySystem.h
        LibrarySystem.cpp
        DataPaths.h
        DataPaths.cpp
)

target_link_libraries(LIBRARY_GUI PRIVATE user32 gdi32 winmm)
