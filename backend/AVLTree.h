
#ifndef AVLTREE_H
#define AVLTREE_H

#include "Book.h"

class AVLTree {
private:

    struct Node {
        Book* book;
        Node* left;
        Node* right;
        int height;

        Node(Book* b) {
            book = b;
            left = nullptr;
            right = nullptr;
            height = 1;
        }
    };

    Node* root = nullptr;

    // Calculate height
    int height(Node* node) {
        return node ? node->height : 0;
    }

    // Calculate balance factor
    int balanceFactor(Node* node) {
        if (!node)
            return 0;

        return height(node->left) - height(node->right);
    }

    // Update node height
    void updateHeight(Node* node) {
        node->height = 1 + max(
            height(node->left),
            height(node->right)
        );
    }

    // Right Rotation (LL Case)
    Node* rotateRight(Node* y) {

        Node* x = y->left;
        Node* t = x->right;

        x->right = y;
        y->left = t;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    // Left Rotation (RR Case)
    Node* rotateLeft(Node* x) {

        Node* y = x->right;
        Node* t = y->left;

        y->left = x;
        x->right = t;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    // Balance AVL Tree
    Node* balance(Node* node) {

        if (!node)
            return node;

        updateHeight(node);

        int bf = balanceFactor(node);

        // LL Case
        if (bf > 1 &&
            balanceFactor(node->left) >= 0) {

            return rotateRight(node);
        }

        // LR Case
        if (bf > 1 &&
            balanceFactor(node->left) < 0) {

            node->left = rotateLeft(node->left);

            return rotateRight(node);
        }

        // RR Case
        if (bf < -1 &&
            balanceFactor(node->right) <= 0) {

            return rotateLeft(node);
        }

        // RL Case
        if (bf < -1 &&
            balanceFactor(node->right) > 0) {

            node->right = rotateRight(node->right);

            return rotateLeft(node);
        }

        return node;
    }

    // Insert node
    Node* insert(Node* node, Book* book) {

        if (!node)
            return new Node(book);

        if (book->id < node->book->id) {

            node->left = insert(node->left, book);
        }

        else if (book->id > node->book->id) {

            node->right = insert(node->right, book);
        }

        else {
            return node;
        }

        return balance(node);
    }

    // Find minimum node
    Node* minNode(Node* node) {

        while (node->left)
            node = node->left;

        return node;
    }

    // Delete node
    Node* remove(Node* node, int id) {

        if (!node)
            return nullptr;

        if (id < node->book->id) {

            node->left = remove(node->left, id);
        }

        else if (id > node->book->id) {

            node->right = remove(node->right, id);
        }

        else {

            // Zero or one child
            if (!node->left || !node->right) {

                Node* child = node->left
                            ? node->left
                            : node->right;

                delete node;

                return child;
            }

            // Two children
            Node* successor = minNode(node->right);

            node->book = successor->book;

            node->right = remove(
                node->right,
                successor->book->id
            );
        }

        return balance(node);
    }

    // Search by Book ID
    Book* search(Node* node, int id) {

        if (!node)
            return nullptr;

        if (id == node->book->id)
            return node->book;

        if (id < node->book->id)
            return search(node->left, id);

        return search(node->right, id);
    }

    // Inorder traversal
    void inorder(Node* node) {

        if (!node)
            return;

        inorder(node->left);

        displayBook(node->book);

        inorder(node->right);
    }

    // Delete all tree nodes
    void clear(Node* node) {

        if (!node)
            return;

        clear(node->left);
        clear(node->right);

        delete node;
    }

public:

    // Destructor
    ~AVLTree() {
        clear(root);
    }

    // Public insertion
    void insert(Book* book) {
        root = insert(root, book);
    }

    // Public deletion
    void remove(int id) {
        root = remove(root, id);
    }

    // Public searching
    Book* search(int id) {
        return search(root, id);
    }

    // Display books sorted by ID
    void displayAll() {

        if (!root) {
            cout << "\nNo books found.\n";
            return;
        }

        inorder(root);
    }
};

#endif
