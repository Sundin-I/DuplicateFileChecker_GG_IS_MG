//
// Created by Hikari on 11/3/2026.
//

#include "Btree_draft.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>

Btree_draft::node::node (bool is_leaf) : is_leaf (is_leaf) {
}

Btree_draft::Btree_draft (int minDegree): root(nullptr), minDegree(minDegree){
    if (minDegree < 2) {
        throw std::invalid_argument("minDegree must be at least 2");
    }
}


void Btree_draft::splitsChildren(node* parent, int index) {
    node* fullChild = parent->children[index].get();

    auto sibling = std::make_unique<node>(fullChild->is_leaf);

    const int middleIndex = minDegree - 1;
    const int maxKeys = 2 * minDegree - 1;

    std::uintmax_t middleKey = fullChild->keys[middleIndex];
    std::vector<std::string> middlePaths = std::move(fullChild->filePaths[middleIndex]);

    for (int i = minDegree; i < maxKeys; i++) {
        sibling->keys.push_back(fullChild->keys[i]);
        sibling->filePaths.push_back(std::move(fullChild->filePaths[i]));
    }

    if (!fullChild->is_leaf) {
        for (int i = minDegree; i < 2 * minDegree; i++) {
            sibling->children.push_back(std::move(fullChild->children[i]));
        }
        fullChild->children.resize(minDegree);
    }

    fullChild->keys.resize(middleIndex);
    fullChild->filePaths.resize(middleIndex);


    parent->keys.insert(parent->keys.begin() + index, middleKey);
    parent->filePaths.insert(parent->filePaths.begin() + index, std::move(middlePaths));
    parent->children.insert(parent->children.begin() + index + 1, std::move(sibling));


}
void Btree_draft::insertingNotFull(node* currentNode, std::uintmax_t fileSize, const std::string & filePath) {
    int index = static_cast<int>(currentNode->keys.size()) - 1;


    if (currentNode->is_leaf) {

        while (index >= 0 && currentNode->keys[index] > fileSize) {
            index--;
        }

        if (index >= 0 && currentNode->keys[index] == fileSize) {
            currentNode->filePaths[index].push_back(filePath);
            return;
        }

        int insertIndex = index + 1;



        currentNode->keys.insert(currentNode->keys.begin() + insertIndex, fileSize);

        currentNode->filePaths.insert(currentNode->filePaths.begin() + insertIndex, std::vector<std::string>{filePath});

        return;
    }
    while (index >= 0 && currentNode->keys[index] > fileSize) {
        index--;
    }

    if (index >= 0 && currentNode->keys[index] == fileSize) {
        currentNode->filePaths[index].push_back(filePath);
        return;
    }
    int childIndex = index + 1;

    const std::size_t maxKeys = static_cast<std::size_t>(2 * minDegree - 1);

    if (currentNode->children[childIndex]->keys.size() == maxKeys) {
        splitsChildren(currentNode, childIndex);
        if (currentNode->keys[childIndex] == fileSize) {
            currentNode->filePaths[childIndex].push_back(filePath);
            return;
        }
        if (fileSize > currentNode->keys[childIndex]) {
            childIndex++;
        }
    }
    insertingNotFull(currentNode->children[childIndex].get(), fileSize, filePath);
}

void Btree_draft::insert(std::uintmax_t fileSize, const std::string & filePath) {
    if (root == nullptr) {
        root = std::make_unique<node>(true);

        root->keys.push_back(fileSize);
        root->filePaths.push_back({filePath});

        return;
    }

    std::vector<std::string> existingPaths = searchNode(root.get(), fileSize);
    if (!existingPaths.empty()) {
        insertingNotFull(root.get(), fileSize, filePath);
        return;
    }

    const std::size_t maxKeys = static_cast<std::size_t>(2 * minDegree - 1);

    if (root->keys.size() == maxKeys) {
        auto newRoot = std::make_unique<node>(false);
        newRoot->children.push_back(std::move(root));
        splitsChildren(newRoot.get(), 0);
        root = std::move(newRoot);
    }
    insertingNotFull(root.get(), fileSize, filePath);
}

void Btree_draft::printInOrder(const node *currentNode) const {
    if (currentNode == nullptr) {
        return;
    }
    for (std::size_t i = 0; i < currentNode->keys.size(); i++) {
        if (!currentNode->is_leaf) {
            printInOrder(currentNode->children[i].get());
        }
        std::cout << "File size: " << currentNode->keys[i] << std::endl;

        for (const std::string& path : currentNode->filePaths[i]) {
            std::cout << "Path: " << path << std::endl;
        }
    }
    if (!currentNode->is_leaf) {
        printInOrder(currentNode->children.back().get());
    }
}

void Btree_draft::printInOrder() const {
    printInOrder(root.get());
    std::cout << '\n';

}



std::vector<std::string> Btree_draft::searchNode(const node* currentNode, std::uintmax_t fileSize) const {
    if (currentNode == nullptr) {
        return {};
    }
    std::size_t index = 0;

    while (index < currentNode->keys.size() && currentNode->keys[index] < fileSize) {
        index++;
    }

    if (index < currentNode->keys.size() && currentNode->keys[index] == fileSize) {
        return currentNode->filePaths[index];
    }

    if (currentNode->is_leaf) {
        return {};
    }
    return searchNode(currentNode->children[index].get(), fileSize);
}


std::vector<std::string> Btree_draft::search(std::uintmax_t fileSize) const {
        return searchNode(root.get(), fileSize);
}

bool Btree_draft::Nodevalidation(const node* currentNode, const std::uintmax_t* lowerBound, const std::uintmax_t* upperBound, int depth, int& leafDepth, bool isRoot) const {
    if (currentNode == nullptr) {
        return false;
    }
    const std::size_t keyCount = currentNode->keys.size();
    const std::size_t maxKeys = static_cast<std::size_t>(2 * minDegree - 1);
    const std::size_t minKeys = static_cast<std::size_t>(minDegree - 1);

    if (keyCount > maxKeys) {
        return false;
    }

    if (!isRoot && keyCount <minKeys) {
        return false;
    }
    if (isRoot && keyCount == 0) {
        return false;
    }
    if (currentNode->filePaths.size() != keyCount) {
        return false;
    }
    for (const auto& paths : currentNode->filePaths) {
        if (paths.empty()) {
            return false;
        }
    }
    for (std::size_t i = 1; i < keyCount; i++) {
        if (currentNode->keys[i-1] >= currentNode->keys[i]) {
            return false;

        }
    }
    for (std::size_t i = 0; i < keyCount; i++) {
        if (lowerBound != nullptr && currentNode->keys[i] <= *lowerBound) {
            return false;
        }
        if (upperBound != nullptr && currentNode->keys[i] >= *upperBound) {
            return false;
        }

    }
    if (currentNode->is_leaf) {
        if (!currentNode->children.empty()) {
            return false;
        }
        if (leafDepth == -1) {
            leafDepth = depth;
        }
        else if (leafDepth != depth) {
            return false;
        }
        return true;
    }
    if (currentNode->children.size() != keyCount +1) {
        return false;
    }
    for (std::size_t i = 0; i < currentNode->children.size(); i++) {
        if (currentNode->children[i].get() == nullptr) {
            return false;
        }
        const std::uintmax_t* childLower = (i == 0) ? lowerBound : &currentNode->keys[i-1];
        const std::uintmax_t* childUpper = (i == keyCount) ? upperBound : &currentNode->keys[i];

        if (!Nodevalidation(currentNode->children[i].get(),childLower, childUpper, depth+1, leafDepth, false)) {
            return false;
        }
    }
    return true;
}

bool Btree_draft::validator() const {
    if (root == nullptr) {
        return true;
    }

    int leafDepth = -1;

    return Nodevalidation(root.get(), nullptr, nullptr, 0, leafDepth, true);
}


