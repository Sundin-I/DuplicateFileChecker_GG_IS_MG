//
// Created by Hikari on 11/3/2026.
//

#ifndef DUPLICATEFILECHECKER_GG_IS_MG_BTREE_DRAFT_H
#define DUPLICATEFILECHECKER_GG_IS_MG_BTREE_DRAFT_H

#include <cstdint>
#include <memory>
#include <string>
#include <vector>


class Btree_draft {
private:


    struct node {
        bool is_leaf;
        std::vector<std::uintmax_t> keys;
        std::vector<std::vector<std::string>> filePaths;
        std::vector<std::unique_ptr<node>> children;
        explicit node (bool is_leaf);
        };
    std::unique_ptr<node> root;
    std::vector<std::string> searchNode(const node* currentNode, std::uintmax_t fileSize) const;
    int minDegree;
    void splitsChildren(node* parent, int index);
    void insertingNotFull(node* currentNode, std::uintmax_t fileSize, const std::string & filePath);
    void printInOrder (const node* currentNode) const;
    bool Nodevalidation(const node* currentNode, const std::uintmax_t* lowerBound, const std::uintmax_t* upperBound, int depth, int& leafDepth, bool isRoot) const;






public:
    explicit Btree_draft (int minDegree = 2);
    void insert(std::uintmax_t fileSize, const std::string & filePath);
    void printInOrder() const;

    bool validator() const;
    std::vector<std::string> search(std::uintmax_t fileSize) const;


};


#endif //DUPLICATEFILECHECKER_GG_IS_MG_BTREE_DRAFT_H


