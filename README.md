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

##Functions

- `insert()` = For inserting
- `search()` = 
- `splitsChildren()`
- `insertingNotFull()`
- `printInOrder()`
- `validator()`
