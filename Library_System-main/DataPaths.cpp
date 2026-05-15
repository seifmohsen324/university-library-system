#include "DataPaths.h"

#include <filesystem>
using namespace std;

// Function to resolve the data file path
string resolveDataFilePath(const string& fileName) {
    namespace fs = std::filesystem;

    const fs::path directPath = fs::current_path() / fileName;
    if (fs::exists(directPath)) {
        return directPath.string();
    }

    const fs::path buildPath = fs::current_path() / "cmake-build-debug" / fileName;
    if (fs::exists(buildPath)) {
        return buildPath.string();
    }

    return buildPath.string();
}
