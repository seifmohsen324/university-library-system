# University Library Management System

A C++ library-management application with a command-line target and a Windows GUI target. The code models books, members, librarians, loans, borrow requests, dates, and JSON-backed data operations.

## Features represented in the source

- Librarian operations for books, members, requests, and loans
- Member operations for requesting and returning books and viewing account/loan information
- Search and listing operations
- JSON-based persistence helpers
- Separate CLI and Windows GUI entry points

## Repository layout

The application source and `CMakeLists.txt` are under `Library_System-main/`. The CMake project defines targets `LIBRARY` and `LIBRARY_GUI`; the GUI uses Win32 libraries, and both targets link `winmm`. The repository also contains a `cmake-build-debug/` directory with previously generated binaries and sample JSON data.

## Build

The checked-in CMake configuration requires CMake 3.26 or later and a Windows toolchain that provides the linked system libraries. From `Library_System-main/`, configure into a new build directory:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

Run the program with the working directory set to `Library_System-main` so the data-path helper can find the sample files under `cmake-build-debug/`. The GUI target is Windows-specific; this repository has not been validated on other platforms.

## Data and limitations

The application reads and writes JSON data. Back up sample data before testing operations that add, edit, or remove records. This is an educational project; review persistence behavior and authorization logic before using it with real member information.
