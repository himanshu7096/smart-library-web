
#ifndef BST_H
#define BST_H

#include "Book.h"
#include <vector>

class BST {
private:

    struct Node {
        Book* book;
        Node* left;
        Node* right;

        Node(Book* b) {
            book = b;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root = nullptr;

    // Compare books by title, then by ID
    bool lessBook(Book* a, Book* b) {
        string x = normalize(a->title);
        string y = normalize(b->title);

        if (x != y)
            return x < y;

        return a->id < b->id;
    }

    // Insert a book into BST
    Node* insert(Node* node, Book* book) {

        if (node == nullptr)
            return new Node(book);

        if (lessBook(book, node->book))
            node->left = insert(node->left, book);

        else
            node->right = insert(node->right, book);

        return node;
    }

    // Find minimum node
    Node* minNode(Node* node) {

        while (node && node->left)
            node = node->left;

        return node;
    }

    // Delete a book from BST
    Node* remove(Node* node, Book* book) {

        if (node == nullptr)
            return nullptr;

        if (lessBook(book, node->book)) {
            node->left = remove(node->left, book);
        }

        else if (lessBook(node->book, book)) {
            node->right = remove(node->right, book);
        }

        else {

            // Case 1: No left child
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            }

            // Case 2: No right child
            if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            }

            // Case 3: Two children
            Node* successor = minNode(node->right);

            node->book = successor->book;

            node->right = remove(
                node->right, successor->book
            );
        }

        return node;
    }

    // Search books by exact title
    void searchTitle(
        Node* node,
        const string& title,
        vector<Book*>& results
    ) {

        if (node == nullptr)
            return;

        string current = normalize(node->book->title);

        if (title < current) {
            searchTitle(node->left, title, results);
        }

        else if (title > current) {
            searchTitle(node->right, title, results);
        }

        else {
            results.push_back(node->book);

            searchTitle(node->left, title, results);
            searchTitle(node->right, title, results);
        }
    }

    // Inorder traversal
    void inorder(Node* node) {

        if (node == nullptr)
            return;

        inorder(node->left);

        displayBook(node->book);

        inorder(node->right);
    }

    // Free BST nodes
    void clear(Node* node) {

        if (node == nullptr)
            return;

        clear(node->left);
        clear(node->right);

        delete node;
    }

public:

    // Destructor
    ~BST() {
        clear(root);
    }

    // Public insertion
    void insert(Book* book) {
        root = insert(root, book);
    }

    // Public deletion
    void remove(Book* book) {
        root = remove(root, book);
    }

    // Public searching
    vector<Book*> search(string title) {

        vector<Book*> results;

        searchTitle(
            root,
            normalize(title),
            results
        );

        return results;
    }

    // Display all books alphabetically
    void displayAll() {

        if (root == nullptr) {
            cout << "\nNo books found.\n";
            return;
        }

        inorder(root);
    }
};

#endif
