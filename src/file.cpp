#include <algorithm>
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>

struct File {
public:
    std::string fileName;
    std::string filePath;
    std::uintmax_t fileSize = 0;
    std::string contentHash = ""; // Only calculate when needed
    std::string extension;

    // FNV-1a 64-bit hash of the file's contents.
    // Reads the file in 64 KB chunks so large files don't need to fit in memory.
    // If the file can't be opened or read, contentHash stays empty and the
    // file is left out of duplicate matching.
    void computeHash() {
        if (!contentHash.empty()) {
            return; // already hashed
        }

        std::ifstream in(filePath, std::ios::binary);
        if (!in) {
            return;
        }

        const std::uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
        const std::uint64_t FNV_PRIME = 1099511628211ULL;
        std::uint64_t hash = FNV_OFFSET_BASIS;

        std::vector<char> buffer(64 * 1024);
        while (in) {
            in.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            std::streamsize bytesRead = in.gcount();
            for (std::streamsize i = 0; i < bytesRead; i++) {
                hash ^= static_cast<unsigned char>(buffer[i]); // XOR first...
                hash *= FNV_PRIME;                             // ...then multiply (the "1a" order)
            }
        }

        if (in.bad()) {
            return; // read error partway through, don't trust a partial hash
        }

        std::ostringstream out;
        out << std::hex << std::setw(16) << std::setfill('0') << hash;
        contentHash = out.str();
    }
};

// Byte-by-byte comparison of two files.
// FNV-1a is fast but not collision-proof, so this is run before any deletion
// to guarantee we never delete a file that only shares a hash by coincidence.
inline bool sameContent(const File& a, const File& b) {
    if (a.fileSize != b.fileSize) {
        return false;
    }

    std::ifstream inA(a.filePath, std::ios::binary);
    std::ifstream inB(b.filePath, std::ios::binary);
    if (!inA || !inB) {
        return false;
    }

    std::vector<char> bufA(64 * 1024);
    std::vector<char> bufB(64 * 1024);
    while (inA && inB) {
        inA.read(bufA.data(), static_cast<std::streamsize>(bufA.size()));
        inB.read(bufB.data(), static_cast<std::streamsize>(bufB.size()));
        std::streamsize nA = inA.gcount();
        std::streamsize nB = inB.gcount();
        if (nA != nB) {
            return false;
        }
        if (!std::equal(bufA.begin(), bufA.begin() + nA, bufB.begin())) {
            return false;
        }
    }
    return inA.eof() && inB.eof();
}
