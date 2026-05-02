#include "StockBST.h"
#include <iostream>
#include <iomanip>

using std::cout;
using std::string;
using std::vector;

StockBST::StockBST() : root(nullptr) {}

StockBST::~StockBST() {
    clear();
}

//constructor
void StockBST::insert(const string& ticker, double key, int year) {
    root = insertHelper(root, ticker, key, year);
}

//destructor
BSTNode* StockBST::search(double key) const {
    return searchHelper(root, key);
}

//search in range
void StockBST::rangeSearch(double low, double high, vector<BSTNode*>& results) const {
    rangeSearchHelper(root, low, high, results);
}

// in order function
void StockBST::inorder() const { 
    inorderHelper(root);   
}

//pre and post order for the stock bst
void StockBST::preorder() const { 
    preorderHelper(root);  
}

void StockBST::postorder() const { 
    postorderHelper(root); 
}

//height
int  StockBST::getHeight() const {
    return heightHelper(root); 
}

//delete the tree
void StockBST::clear() {
    clearHelper(root);
    root = nullptr;
}

//check empty
bool StockBST::isEmpty() const { 
    return root == nullptr; 
}

//helper function to insert a node into the tree
BSTNode* StockBST::insertHelper(BSTNode* node, const string& ticker, double key, int year) {
    //insertion
    if (node == nullptr) return new BSTNode(ticker, key, year);

    //move left to insert node
    if (key < node->key) {
        node->left  = insertHelper(node->left,  ticker, key, year);
    } else {
        //go right if key greater
        node->right = insertHelper(node->right, ticker, key, year);
    }
    
    return node;
}

//search helper
BSTNode* StockBST::searchHelper(BSTNode* node, double key) const {
    if (node == nullptr) return nullptr;    //empty branch
    if (key == node->key) return node;      //if node match return it
    if (key < node->key) return searchHelper(node->left,  key);
    return searchHelper(node->right, key);
}

//
void StockBST::rangeSearchHelper(BSTNode* node, double low, double high, vector<BSTNode*>& results) const {
    if (node == nullptr) return;    //empty branch 

    if (low < node->key) rangeSearchHelper(node->left, low, high, results); //left contains smaller
    if (node->key >= low && node->key <= high) results.push_back(node); //if fits the range then add
    if (high >= node->key) rangeSearchHelper(node->right, low, high, results); //right contains larger
}

//in order traverse
void StockBST::inorderHelper(BSTNode* node) const {
    if (node == nullptr) return;   //empty branch
    //go left, check, go right, repeat 
    inorderHelper(node->left);
    cout << node->ticker  << node->year << std::fixed << node->key << "%\n";
    inorderHelper(node->right);
}

void StockBST::preorderHelper(BSTNode* node) const {
    if (node == nullptr) return; //empty branch
    //check then progress
    cout << node->ticker << node->year << node->key << "%\n";
    preorderHelper(node->left);
    preorderHelper(node->right);
}

void StockBST::postorderHelper(BSTNode* node) const {
    if (node == nullptr) return; //empty branch
    //progress then check while returning back up
    postorderHelper(node->left);
    postorderHelper(node->right);
    cout << node->ticker << node->year << node->key << "%\n";
}

int StockBST::heightHelper(BSTNode* node) const {
    if (node == nullptr) return 0; //empty branch
    int leftH  = heightHelper(node->left); //left heihgt
    int rightH = heightHelper(node->right); //right height
    
    //which height is valid (take the greater)
    int maxHeight;

    if (leftH > rightH) {
        maxHeight = leftH;
    } else {
        maxHeight = rightH;
    }

    return 1 + maxHeight;
}

//clear helper, go through the nodes of the tree
void StockBST::clearHelper(BSTNode* node) {
    if (node == nullptr) return;
    clearHelper(node->left);
    clearHelper(node->right);
    delete node;
}