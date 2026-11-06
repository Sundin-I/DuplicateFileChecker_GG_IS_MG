#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

#include "src/Btree_draft.h"

namespace fs = std::filesystem;
using namespace std;

int main(){

	fs::path dataset_path = R"(H:\_UF files\Summer 2026\gen_images)";

	Btree_draft bt;

	size_t totalFiles = 0;
	size_t skippedFiles = 0;
	size_t searchFailures = 0;
	size_t searchTests = 0;

	unordered_map<uintmax_t, size_t> sizeFrequency;
	unordered_map<string, size_t> extensionFrequency;

	try {
		cout << "Beginning dataset scan..." << endl;

		for (const auto& entry : fs::directory_iterator(dataset_path)) {
			if (!entry.is_regular_file()) {
				continue;
			}
			error_code ec;
			uintmax_t fileSize = fs::file_size(entry.path(), ec);

			if (ec) {
				skippedFiles++;
				continue;
			}

			string filePath = entry.path().string();

			string fileType = entry.path().extension().string();

			transform(fileType.begin(), fileType.end(), fileType.begin(), [](unsigned char c) {
				return static_cast<char>(tolower(c));
			});
			bt.insert(fileSize, filePath);

			sizeFrequency[fileSize]++;
			extensionFrequency[fileType]++;

			totalFiles++;

			if (totalFiles % 10000 == 0) {
				cout << "Processed " << totalFiles << " files..." << endl;
			}
		}
	}
	catch (const fs::filesystem_error& e) {
		cerr << "Filesystem error: " << e.what() << endl;
		return 1;
	}
	cout << endl << "====DATASET RESULTS====" << endl;
	cout << "Total files processed: " << totalFiles << endl;
	cout << "Total files skipped: " << skippedFiles << endl;
	cout << "Unique file sizes:" << sizeFrequency.size() << endl;
	cout << "File types found:" << extensionFrequency.size() << endl;


	cout << endl << "====FILE TYPE DISTRIBUTION====" << endl;

	for (const auto& [extension, count] : extensionFrequency) {
		cout << (extension.empty() ? "[no extension]" : extension) << ": " << count << endl;
	}
	size_t duplicateSizeGroups = 0;

	for (const auto& [size, count] : sizeFrequency) {
		if (count > 1) {
			vector<string> results = bt.search(size);
			searchTests++;

			if (results.size() != count) {
				searchFailures++;
			}
		}
	}
	cout << "Search groups tested:" << searchTests << endl;
	cout << "Search groups failed:" << searchFailures << endl;


	cout << endl << "====DUPLICATE SIZE RESULTS====" << endl;
	cout << "Group sharing file sizes: " << duplicateSizeGroups << endl;

	cout << endl << "====B-tree VALIDATION====" << endl;
	cout << "B-tree valid: " << bt.validator() << endl;

	cout << endl << "====DUPLICATE SEARCH TEST====" << endl;

	bool searchTested = false;

	for (const auto& [size, count] : sizeFrequency) {
		if (count > 1) {
			vector<string> results = bt.search(size);

			cout << "Searching for file size: " << size << endl;
			cout << "Expected path: " << count << endl;
			cout << "Retrieved path: " << results.size() << endl;

			for (size_t i = 0; i < min(results.size(), size_t(5)); i++) {
				cout << results[i] << endl;
			}
			searchTested = true;
			break;
		}
	}
	if (!searchTested) {
		cout << "No repeated file size were found." << endl;
	}

	return 0;

}
