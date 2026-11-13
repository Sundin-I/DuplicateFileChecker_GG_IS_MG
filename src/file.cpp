#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>

struct File {
public:
    std::string fileName;
    std::string filePath;
    uintmax_t fileSize;
    std::string contentHash = ""; // Only calculate when needed
    std::string extension;

    void computeHash() {
        // TODO: Implement
    }
};
