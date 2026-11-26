#include "src/Btree_draft.h"

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <chrono>

using namespace std;
namespace fs = filesystem;

struct FileLexicon {
  uintmax_t size_of_file;
  string path_of_file;
};

int main() {
	string pathInput;
	cout << "Please enter the path to the dataset:" << endl;
	getline(cin, pathInput);
    fs::path dataset_path = fs::path(pathInput);
    
	if (!fs::exists(dataset_path) || !fs::is_directory(dataset_path)) {
        cerr << "Error: The provided path does not exist or is invalid." << endl;
        return 1;
    }

    vector<FileLexicon> records;

    for (const auto& entry : fs::directory_iterator(dataset_path)) {
      if (!entry.is_regular_file()) {
        continue;
      }
      error_code errorCode;
      uintmax_t size = fs::file_size(entry.path(), errorCode);

      if (errorCode) {
        continue;
      }
      records.push_back({size, entry.path().string()});
    }
  cout << endl << "Files collected from dataset: " << records.size() << endl;

  double Isabelle_Btree_totalMS = 0;
  double Gabriel_hash_totalMS = 0;

  const int trials = 10;

  for (int run = 1; run <= trials; run++) {
    Btree_draft bt;
    auto BtreeBeginning = chrono::steady_clock::now();

    for (const auto& file : records) {
      bt.insert(file.size_of_file, file.path_of_file);
    }
    auto BtreeEnding = chrono::steady_clock::now();

    double BtreeMS = chrono::duration<double, milli> (BtreeEnding - BtreeBeginning).count();

    Isabelle_Btree_totalMS += BtreeMS;

    unordered_map<uintmax_t, vector<string>> groups_of_hashes;

    auto hashBeginning = chrono::steady_clock::now();
    
    for (const auto& file : records) {
      groups_of_hashes[file.size_of_file].push_back(file.path_of_file);
    }
    auto hashEnding = chrono::steady_clock::now();

    double HashMS = chrono::duration<double, milli> (hashEnding - hashBeginning).count();

    Gabriel_hash_totalMS += HashMS;

    bool valid = bt.validator();
    bool matchingGroups = true;

    if (groups_of_hashes.empty() && !records.empty()) {
      matchingGroups = false;
    }

    for (const auto& [size, paths] : groups_of_hashes) {
      vector<string> btreePaths = bt.search(size);
      vector<string> expectedPaths = paths;

      sort(btreePaths.begin(), btreePaths.end());
      sort(expectedPaths.begin(), expectedPaths.end());

      if (btreePaths != expectedPaths) {
        matchingGroups = false;
        break;
      }
    }
    cout << endl << "Trial " << run << endl;
    cout << "B-tree insertion: " << BtreeMS << "ms" << endl;
    cout << "Hashmap insertion: " << HashMS << "ms" << endl;    
    cout << "Btree valid: " << valid << endl;
    cout << "Groups match: " << matchingGroups <<endl;

    if (!valid || !matchingGroups) {
      cerr << "Benchmark correctness check failed." <<endl;
      return 1;
    }
  }
  cout << endl<< "-----AVERAGE RESULTS------" << endl;
  cout << "Files processed: " << records.size() << endl;
  cout << "Average Btree insertion: " << Isabelle_Btree_totalMS/trials << "ms" << endl;
  cout << "Average hashmap insertion: " << Gabriel_hash_totalMS/trials << "ms" << endl;

  cout << "Unique file sizes: ";

  unordered_map<uintmax_t, bool> uniqueSizes;

  for (const auto& files : records) {
    uniqueSizes[files.size_of_file] = true;
  }
  cout << uniqueSizes.size() << endl;

  cout << "All correctness checks passed." << endl;

  return 0 ;

    
}