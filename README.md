# COP3530 Project 3 - Duplicate Checker
### B-tree implementation portion


## Summary
This is my implementation a Btree for our duplicate checker. 
As intended it will organize file by their file sizes...unfortunately that's the most that's been developed. 
To elaborate, I'm still trying to find a dataset with differing types, sizes, and surmounts to 100K files. 
Beyond that, here's a quick explanation:


## Configuration
- min. degree: t = 2
- Key: File size(std::uintmax_t)
- Associated values(std::vector<std::string>)
- max. keys per node: 2t - 1 (for example, 3 keys when t = 2)
- max. keys per node: t - 1 (for example, 1 key when t = 2)

## Structure
Each node will contain:
- is_leaf: as the name implies, confirms is a node is a leaf
- keys: the files sizes will be stored in sorted order
- filePaths: the files paths will be stored to align with each file size key
- children: the pointer will be stored with children for internal nodes

  **Files with the same size will share one keys for their paths are stored together in the associated filePaths entry**

## Functions

- `insert()` = For inserting each file size & its matching file path in the Btree
- `search()` = For searching for the inserted file size & ends up returning the file path 
- `splitsChildren()` = For balancing by splitting a full child node before making the median key a parent
- `insertingNotFull()` = As the specific naming convention states, a file size/filepath will be inserted to a non-full node 
- `printInOrder()` = I chose to use the Inorder traversal for the Bree's sorted file size order
- `validator()` = My crazy amount of testers/structure properties checkers

## Handing Duplicate File Sizes
Upon inserting the needed files, the Btree will check if the file size already existed
- Nonexistent? A new key & path will be inserted
- Existent? A brand new path will be appended to the already existing

**Note: Additional keys won't be given to duplicate file sizes**

## How to start duplicate checking

### Use the Btree header:
```#include "Btree_draft.h"```

### Create a Btree:
```Btree_draft BT;```

### Call insert:
```BT.insert(328, "$500 bill (U.S.).png");```

```BT.insert(64, "1mg.png");```

```BT.insert(64, "1_2mg.png");```

```BT.insert(95, "& Other Stories.png");```

### Call search:
```std::vector<std::string> results = BT.search(64);
for (const std::string& path : results) {
    std::cout <<path << std::endl;
    }
```
### Don't forget to validate your tree
std::cout << BT.validator()<< std::endl;

**If the structural validation passed, the output should be 1**

### Should output
```
1mg.png
1_2mg.png
1
```

## Main.cpp expected output when testing the 100K image dataset

```
"H:\_UF files\Summer 2026\DuplicateFileChecker_GG_IS_MG\cmake-build-debug\Main.exe"
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
Unique file sizes:80932
File types found:1

====FILE TYPE DISTRIBUTION====
.png: 104611
Search groups tested:19375
Search groups failed:0

====DUPLICATE SIZE RESULTS====
Group sharing file sizes: 0

====B-tree VALIDATION====
B-tree valid: 1

====DUPLICATE SEARCH TEST====
Searching for file size: 124207
Expected path: 2
Retrieved path: 2
H:\_UF files\Summer 2026\gen_images\Women in Baseball.png
H:\_UF files\Summer 2026\gen_images\Yanka Kupala State University of Grodno.png

Process finished with exit code 0
```
