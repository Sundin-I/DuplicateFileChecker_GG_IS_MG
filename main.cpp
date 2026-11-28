#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <map>
#include <exception>

#include "src/Btree_draft.h"
#include "src/file.cpp"

using namespace std;
namespace fs = filesystem;

int main(){
	string pathInput;
	cout << "Please enter the path to the dataset:" << endl;
	getline(cin, pathInput);
	fs::path dataset_path = fs::path(pathInput);

	if (!fs::exists(dataset_path) || !fs::is_directory(dataset_path)) {
        cerr << "Error: The provided path does not exist or is invalid." << endl;
        return 1;
    }

	Btree_draft bt;

	size_t totalFiles = 0;
	size_t skippedFiles = 0;
	vector<File> allFiles;
	allFiles.reserve(1000000); // Reserving space for 1 mil files to reduce reallocations

	unordered_map<string, vector<File*>> fileGroups;
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

			File newFile;
			newFile.fileName = entry.path().filename().string();
			newFile.filePath = entry.path().string();
			newFile.fileSize = fileSize;

			string fileType = entry.path().extension().string();

			transform(fileType.begin(), fileType.end(), fileType.begin(), [](unsigned char c) {
				return static_cast<char>(tolower(c));
			});
			newFile.extension = fileType;
			allFiles.push_back(newFile);
			File* filePtr = &allFiles.back();

			bt.insert(newFile.fileSize, newFile.filePath);

			// Groupting files by size and extension as the potential duplicates
			string key = to_string(newFile.fileSize) + "_" + newFile.extension;
            fileGroups[key].push_back(filePtr);

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

	// Report initial dataset scan results
	cout << endl << "====DATASET RESULTS====" << endl;
	cout << "Total files processed: " << totalFiles << endl;
	cout << "Total files skipped: " << skippedFiles << endl;
	cout << "File types found:" << extensionFrequency.size() << endl;
	cout << "Total unique size+extension keys: " << fileGroups.size() << endl;
	cout << "Analyzing potential duplicate groups..." << endl;

	try {
        size_t confirmedDuplicateFilesTotal = 0;
        struct DuplicateSet {
            string hash;
            vector<File*> files;
        };
        vector<DuplicateSet> allDuplicateSets;
        
        // Filter out groups with only 1 file for a smaller dataset
        for (auto& [key, group] : fileGroups) {
            if (group.size() <= 1)  {
                continue;
            }

            // Hash files in this group
            for (File* filePtr : group) {
                filePtr->computeHash();
            }

            unordered_map<string, vector<File*>> hashGroups;
            hashGroups.reserve(group.size());

            for (File* filePtr : group) {
                if (!filePtr->contentHash.empty()) {
                    hashGroups[filePtr->contentHash].push_back(filePtr);
                }
            }

            // Collect confirmed duplicates
            for (auto& [hash, exactMatchGroup] : hashGroups) {
                if (exactMatchGroup.size() > 1) {
                    allDuplicateSets.push_back({hash, move(exactMatchGroup)});
                }
            }
        }


        // Iterates through confirmed duplicates to display them with an id for user selection
        map<int, vector<File*>> confirmedDuplicateGroups;
        int duplicateSetIndex = 1;
        uintmax_t totalWastedBytes = 0;

        for (auto& dupSet : allDuplicateSets) {
            confirmedDuplicateGroups[duplicateSetIndex] = dupSet.files;

            cout << "\n-==== DUPLICATE GROUP #" << duplicateSetIndex++ << " ====" << endl;
            // TODO: Remove the hash in the output, I am only leaving it for debugging purposes for now
            cout << "Hash: " << dupSet.hash << endl;
            cout << "Matching Files:" << endl;
            for (File* dupFile : dupSet.files) {
                cout << "   " << dupFile->filePath << endl;
                confirmedDuplicateFilesTotal++;
            }
            // Every copy beyond the first is wasted space
            totalWastedBytes += (dupSet.files.size() - 1) * dupSet.files.front()->fileSize;
        }

        // Report duplicate analysis results
        cout << endl << "====DUPLICATE ANALYSIS RESULTS====" << endl;
        cout << "Confirmed duplicate groups found: " << confirmedDuplicateGroups.size() << endl;
        cout << "Confirmed duplicate files (via hash match): " << confirmedDuplicateFilesTotal << endl;
        cout << "Total space wasted by duplicates: " << totalWastedBytes << " bytes" << endl;

        // This is the interface where the user will inspect duplicate groups and decide which to delete from
        if (!confirmedDuplicateGroups.empty()) {
            int chosenKey;
            size_t sessionDeletedCount = 0;
            uintmax_t sessionFreedBytes = 0;
            vector<string> sessionDeletedPaths;

            cout << "\nWhich duplicate group would you like to delete from? (Enter the group number or 0 to exit): ";

            while (cin >> chosenKey && chosenKey != 0) {
                auto it = confirmedDuplicateGroups.find(chosenKey);

                if (it == confirmedDuplicateGroups.end()) {
                    cout << "Invalid duplicate group number. Please try again." << endl;
                    cout << "\nEnter another group number to delete from (or 0 to exit): ";
                    continue;
                }

                vector<File*>& group = it->second;
                if (group.size() < 2) {
                    cout << "Group #" << chosenKey << " has no duplicates left." << endl;
                    cout << "\nEnter another group number to delete from (or 0 to exit): ";
                    continue;
                }

                // 1. List the group with numbers so the user can pick a file to keep
                cout << "\nDuplicate Group #" << chosenKey << " (" << group[0]->fileSize << " bytes each)" << endl;
                for (size_t i = 0; i < group.size(); i++) {
                    cout << "   [" << i + 1 << "] " << group[i]->filePath << endl;
                }
                cout << "Enter the number of the file to KEEP (0 to cancel): ";

                int keepChoice;
                if (!(cin >> keepChoice)) {
                    break;
                }
                if (keepChoice < 1 || keepChoice > static_cast<int>(group.size())) {
                    cout << "Cancelled. No files deleted." << endl;
                    cout << "\nEnter another group number to delete from (or 0 to exit): ";
                    continue;
                }
                File* keeper = group[keepChoice - 1];

                // 2. Build the delete list, verifying each file byte-by-byte against the keeper
                vector<File*> toDelete;
                uintmax_t bytesToFree = 0;
                for (File* filePtr : group) {
                    if (filePtr == keeper) {
                        continue;
                    }
                    if (sameContent(*keeper, *filePtr)) {
                        toDelete.push_back(filePtr);
                        bytesToFree += filePtr->fileSize;
                    } else {
                        cout << "   Skipping (content differs or unreadable): " << filePtr->filePath << endl;
                    }
                }

                if (toDelete.empty()) {
                    cout << "Nothing to delete in this group." << endl;
                    cout << "\nEnter another group number to delete from (or 0 to exit): ";
                    continue;
                }

                cout << "\nKeeping: " << keeper->filePath << endl;
                cout << "The following " << toDelete.size() << " file(s) will be deleted ("
                     << bytesToFree << " bytes):" << endl;
                for (const File* filePtr : toDelete) {
                    cout << "   " << filePtr->filePath << endl;
                }

                // 3. Second input to confirm
                cout << "Type DELETE to confirm, anything else to cancel: ";
                string confirm;
                if (!(cin >> confirm)) {
                    break;
                }
                if (confirm != "DELETE") {
                    cout << "Cancelled. No files deleted." << endl;
                    cout << "\nEnter another group number to delete from (or 0 to exit): ";
                    continue;
                }

                // 4. Delete only the duplicates and report what happened
                size_t deletedCount = 0;
                uintmax_t freedBytes = 0;
                vector<File*> remaining;
                for (File* filePtr : group) {
                    bool shouldDelete = find(toDelete.begin(), toDelete.end(), filePtr) != toDelete.end();
                    if (!shouldDelete) {
                        remaining.push_back(filePtr);
                        continue;
                    }
                    error_code ec;
                    if (fs::remove(filePtr->filePath, ec)) {
                        cout << "   Deleted: " << filePtr->filePath << endl;
                        deletedCount++;
                        freedBytes += filePtr->fileSize;
                        sessionDeletedPaths.push_back(filePtr->filePath);
                    } else {
                        cout << "   FAILED to delete: " << filePtr->filePath
                             << (ec ? " (" + ec.message() + ")" : " (file not found)") << endl;
                        remaining.push_back(filePtr);
                    }
                }
                group = move(remaining); // so the same group can't be deleted from twice

                sessionDeletedCount += deletedCount;
                sessionFreedBytes += freedBytes;
                cout << "Deleted " << deletedCount << " file(s), freed " << freedBytes << " bytes." << endl;

                cout << "\nEnter another group number to delete from (or 0 to exit): ";
            }

            // 5. Session summary
            cout << endl << "====DELETION SUMMARY====" << endl;
            cout << "Total files deleted: " << sessionDeletedCount << endl;
            cout << "Total space freed: " << sessionFreedBytes << " bytes" << endl;
            for (const string& path : sessionDeletedPaths) {
                cout << "   " << path << endl;
            }
        }
    }
    catch (const exception& e) {
        cerr << "Error during duplicate analysis: " << e.what() << endl;
        return 1;
    }

    // B-Tree Validation
    cout << endl << "====B-tree VALIDATION====" << endl;
    cout << "B-tree valid: " << bt.validator() << endl;

    return 0;
}
