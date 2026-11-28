# COP3530 Project 3 - Duplicate Checker


## Reason for the implementation
- When working with large collections of images and other files, there will be cases of duplicate files that consume unnecessary storage space. It's quite taxing to look for identical files when filenames aren't very distinguishable to go by.
- As a result, Team GIMfinity of University of Florida came up with a solution.

##Team GIMfinity:
- Isabelle Sundin starting things off with the B tree implementation and created the performanceComparision().
- Gabriel Goluch // add here
- Madasion Garrison //


## Overview
- This is an implementation of a program that checks for duplicate files by scanning a directory that the user provides, organizing the file's information, points out the duplicates, and has the addition function to delete any copy.
- This uses a B_tree as a baseline as it organizes files by their sizes. File sizes are stored as keys while file paths will be stored with it.
## Additional Performance Comparison
- A separate algorithm that compares the performance/runtime of the Btree and hashmap

## Notable Features
- A user specified directory is read to collect file names, paths, sizes, and extensions.
- A B-tree is used to organize file paths based on their associated file sizes.
- Potential duplicates get grouped by matching file size & extension. This is by using an unordered map.
- The FNV-1a 64-bit hashing algorithm identifies files with matching content hashes.
- Potential duplicate groups will be displayed with identifiers which will allow users to select the groups to inspect
- After an explicit DELETE confirmation, users can safely delete their chosen files.
- There will be a consistent report on how many files were deleted in addition to however much storage space was freed.
**Matching file hashes would be used to identify duplicate candidates. Prior to deleting, there going to be a byte-by-byte comparison using sameContent() to confirm whether the files have identical data.

## The Btree - implemented by Isabelle Sundin

### Configuration
- min. degree: t = 2
- Key: File size(std::uintmax_t)
- Associated values(std::vector<std::string>)
- max. keys per node: 2t - 1 (for example, 3 keys when t = 2)
- min. keys per node: t - 1 (for example, 1 key when t = 2)

### Node Structure
Each node will contain:
- is_leaf: as the name implies, confirms is a node is a leaf
- keys: the files sizes will be stored in sorted order
- filePaths: the files paths will be stored to align with each file size key
- children: the pointer will be stored with children for internal nodes

  **Files with the same size will share one key for their paths are stored together in the associated filePaths entry**

### Functions

- `insert()` = For inserting each file size & its matching file path in the Btree
- `search()` = For searching for the inserted file size & ends up returning the file path 
- `splitsChildren()` = For balancing by splitting a full child node before making the median key a parent
- `insertingNotFull()` = As the specific naming convention states, a file size/filepath will be inserted to a non-full node 
- `printInOrder()` = Inorder traversal for the Bree's sorted file size order
- `validator()` = My crazy amount of testers/structure properties checkers.

### Handing Duplicate File Sizes
Upon inserting the needed files, the Btree will check if the file size already existed
- Nonexistent? A new key & path will be inserted
- Existent? A brand new path will be appended to the already existing
**Note: Additional keys won't be given to duplicate file sizes**
  
### Time Complexity
- inserting individually: O(log N)
- searching individually: O(log N)
- inserting N files individually: O(N log N), where N = number of unique file-size keys in the tree

//TODO for Madasin: summarize your overall contribuions and explain how your part works

//TODO for Gabriel: summarize your overall contribuions and explain how your part works

## To Run the overall program:
- You'll need a C++17-compatible compiler(GCC/MinGW), C++ standard library and filesystem support, and a directory of files you want to scan in the first place.

### Compiling
1. Start on a repository root directory and run:
- For Windows(MinGW):
```
g++ -std=c++17 -O2 -static -static-libgcc -static-libstdc++ main.cpp src\Btree_draft.cpp -o main.exe
```
- Linux/macOS(GCC):
```
g++ -std=c++17 -O2 main.cpp src\Btree_draft.cpp -o main
```
src/file.cpp is included in the application and shouldn't be compiled as a separate source file
2. Run the following:
- For Windows(MinGW):
```
main.exe
```
- Linux/macOS(GCC):
```
./main
```
3. You shall be prompted with the following:
```
Please enter the path to the dataset:
\\user types here\\
```

//TODO: Gabriel do me a favor and provide an explain on how to run your deletion workflow.




## Main.cpp expected output when testing the 100K image dataset (the 100k-GenAI-Images-GPTKB from HuggingFace)

```
Beginning dataset scan...
Processed 10000 files...
Processed 20000 files...
Processed 30000 files...
Processed 40000 files...
Processed 50000 files...
Processed 60000 files...
Processed 70000 files...
Processed 80000 files...
Processed 90000 files...
Processed 100000 files...

====DATASET RESULTS====
Total files processed: 104611
Total files skipped: 0
File types found:1
Total unique size+extension keys: 80932
Analyzing potential duplicate groups...

====DUPLICATE ANALYSIS RESULTS====
Confirmed duplicate groups found: 0
Confirmed duplicate files (via hash match): 0
Total space wasted by duplicates: 0 bytes

====B-tree VALIDATION====
B-tree valid: 1

E:\Project\DuplicateFileChecker_GG_IS_MG-main>main.exe
Please enter the path to the dataset:
E:\Demo\gen_images
Beginning dataset scan...
Processed 10000 files...
Processed 20000 files...
Processed 30000 files...
Processed 40000 files...
Processed 50000 files...
Processed 60000 files...
Processed 70000 files...
Processed 80000 files...
Processed 90000 files...
Processed 100000 files...

====DATASET RESULTS====
Total files processed: 104611
Total files skipped: 0
File types found:1
Total unique size+extension keys: 80932
Analyzing potential duplicate groups...

====DUPLICATE ANALYSIS RESULTS====
Confirmed duplicate groups found: 0
Confirmed duplicate files (via hash match): 0
Total space wasted by duplicates: 0 bytes

====B-tree VALIDATION====
B-tree valid: 1

```

  
## Performance Benchmark
The newly committed file "performanceComparision.cpp," effectively compares the insertion and grouping performance of Isabelle's Btree and Gabriel's hashmap. Note: Both data structures would be using the safe 100K dataset.

### To Complie:
From the project's root directory, open a run a terminal and run:
```
g++ -std=c++17 -O2 src/performanceComparision.cpp src/Btree_draft.cpp -o performanceComparision
```
### To Run:
For Windows run the following executable:
```
performanceComparision
```
For Linux/macOS:
```
./performanceComparision
```
### The program's overall tasks:
1. Collects file sizes + paths
2. Insert the records (in this case, the 100K images) in both data structures
3. Actively measures insertion time
4. Repeats step 3 for at least 10 trials
5. validator is constantly keeping tabs
6. Compares the groups coming from the data structures
7. In the aftermath, there are displayed individual trial times and average performance results.

## Expected Output based on 10 trials:
```
Files collected from dataset: 104611

Trial 1
B-tree insertion: 387.649ms
Hashmap insertion: 69.5792ms
Btree valid: 1
Groups match: 1

Trial 2
B-tree insertion: 169.963ms
Hashmap insertion: 92.3739ms
Btree valid: 1
Groups match: 1

Trial 3
B-tree insertion: 208.345ms
Hashmap insertion: 59.208ms
Btree valid: 1
Groups match: 1

Trial 4
B-tree insertion: 224.943ms
Hashmap insertion: 96.8444ms
Btree valid: 1
Groups match: 1

Trial 5
B-tree insertion: 998.134ms
Hashmap insertion: 173.88ms
Btree valid: 1
Groups match: 1

Trial 6
B-tree insertion: 379.052ms
Hashmap insertion: 102.123ms
Btree valid: 1
Groups match: 1

Trial 7
B-tree insertion: 673.158ms
Hashmap insertion: 238.722ms
Btree valid: 1
Groups match: 1

Trial 8
B-tree insertion: 979.29ms
Hashmap insertion: 422.251ms
Btree valid: 1
Groups match: 1

Trial 9
B-tree insertion: 480.82ms
Hashmap insertion: 117.209ms
Btree valid: 1
Groups match: 1

Trial 10
B-tree insertion: 362.106ms
Hashmap insertion: 110.628ms
Btree valid: 1
Groups match: 1

-----AVERAGE RESULTS------
Files processed: 104611
Average Btree insertion: 486.346ms
Average hashmap insertion: 148.282ms
Unique file sizes: 80932
All correctness checks passed.
```


## Average insertion runtime between both data structure:
- B Tree: 486.346 ms
- Hash Map: 148.282 ms (approximately 3.03 times faster)

