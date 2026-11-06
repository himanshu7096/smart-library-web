
#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "BST.h"
#include "AVLTree.h"
#include "HashTable.h"

#include <fstream>
#include <iomanip>
#include <list>
#include <algorithm>

class Library {
private:

    // Central storage of book records
    list<Book> books;

    // Three data structures
    BST bst;
    AVLTree avl;
    HashTable hashTable;

    const string filename = "books.txt";

    // Insert book into all three indexes
    void indexBook(Book* book) {
        bst.insert(book);
        avl.insert(book);
        hashTable.insert(book);
    }

    // Save all books to file
    void saveToFile() {

        ofstream out(filename);

        if (!out) {
            cout << "\nError saving library data!\n";
            return;
        }

        for (const Book& b : books) {

            out << b.id << " "
                << quoted(b.title) << " "
                << quoted(b.author) << " "
                << quoted(b.publisher) << " "
                << b.issued << "\n";
        }

        cout << "\nLibrary data saved successfully.\n";
    }

    // Load books from file
    void loadFromFile() {

        ifstream in(filename);

        if (!in)
            return;

        int id;
        string title, author, publisher;
        bool issued;

        while (in >> id
                  >> quoted(title)
                  >> quoted(author)
                  >> quoted(publisher)
                  >> issued) {

            // Prevent duplicate IDs
            if (hashTable.search(id))
                continue;

            if (id <= 0 || title.empty())
                continue;

            books.emplace_back(
                id, title, author, publisher
            );

            Book* book = &books.back();

            book->issued = issued;

            indexBook(book);
        }

        cout << "\nLibrary records loaded successfully.\n";
    }

public:

    // Constructor
    Library() {
        loadFromFile();
    }

    // =================================
    // ADD BOOK
    // =================================

    void addBook(
        int id,
        string title,
        string author,
        string publisher
    ) {

        if (id <= 0 || title.empty()) {
            cout << "\nInvalid Book ID or Title!\n";
            return;
        }

        if (hashTable.search(id)) {
            cout << "\nBook ID already exists!\n";
            return;
        }

        books.emplace_back(
            id, title, author, publisher
        );

        Book* book = &books.back();

        indexBook(book);

        saveToFile();

        cout << "\nBook added successfully!\n";
    }

    // =================================
    // SEARCH BY HASH TABLE
    // =================================

    void searchByHash(int id) {

        Book* book = hashTable.search(id);

        if (book)
            displayBook(book);

        else
            cout << "\nBook not found!\n";
    }

    // =================================
    // SEARCH BY AVL TREE
    // =================================

    void searchByAVL(int id) {

        Book* book = avl.search(id);

        if (book)
            displayBook(book);

        else
            cout << "\nBook not found!\n";
    }

    // =================================
    // SEARCH BY TITLE USING BST
    // =================================

    void searchByTitle(string title) {

        vector<Book*> results = bst.search(title);

        if (results.empty()) {
            cout << "\nBook not found!\n";
            return;
        }

        cout << "\nMatching Books: "
             << results.size() << "\n";

        for (Book* book : results)
            displayBook(book);
    }

    // =================================
    // UPDATE BOOK
    // =================================

    void updateBook(int id) {

        Book* book = hashTable.search(id);

        if (!book) {
            cout << "\nBook not found!\n";
            return;
        }

        string title, author, publisher;

        cout << "Enter new title: ";
        getline(cin >> ws, title);

        if (title.empty()) {
            cout << "\nTitle cannot be empty!\n";
            return;
        }

        cout << "Enter new author: ";
        getline(cin, author);

        cout << "Enter new publisher: ";
        getline(cin, publisher);

        // Remove old title index
        bst.remove(book);

        // Update central book record
        book->title = title;
        book->author = author;
        book->publisher = publisher;

        // Insert updated title index
        bst.insert(book);

        saveToFile();

        cout << "\nBook updated successfully!\n";
    }

    // =================================
    // DELETE BOOK
    // =================================

    void deleteBook(int id) {

        Book* book = hashTable.search(id);

        if (!book) {
            cout << "\nBook not found!\n";
            return;
        }

        // Remove from all three structures
        bst.remove(book);
        avl.remove(id);
        hashTable.remove(id);

        // Remove central record
        auto it = find_if(
            books.begin(),
            books.end(),
            [id](const Book& b) {
                return b.id == id;
            }
        );

        if (it != books.end())
            books.erase(it);

        saveToFile();

        cout << "\nBook deleted successfully!\n";
    }

    // =================================
    // ISSUE BOOK
    // =================================

    void issueBook(int id) {

        Book* book = hashTable.search(id);

        if (!book) {
            cout << "\nBook not found!\n";
            return;
        }

        if (book->issued) {
            cout << "\nBook is already issued!\n";
            return;
        }

        book->issued = true;

        saveToFile();

        cout << "\nBook issued successfully!\n";
    }

    // =================================
    // RETURN BOOK
    // =================================

    void returnBook(int id) {

        Book* book = hashTable.search(id);

        if (!book) {
            cout << "\nBook not found!\n";
            return;
        }

        if (!book->issued) {
            cout << "\nBook is already available!\n";
            return;
        }

        book->issued = false;

        saveToFile();

        cout << "\nBook returned successfully!\n";
    }

    // =================================
    // DISPLAY BOOKS BY TITLE
    // =================================

    void displayByTitle() {

        cout << "\nBOOKS SORTED BY TITLE\n";

        bst.displayAll();
    }

    // =================================
    // DISPLAY BOOKS BY ID
    // =================================

    void displayByID() {

        cout << "\nBOOKS SORTED BY ID\n";

        avl.displayAll();
    }

    // =================================
    // LIBRARY STATISTICS
    // =================================

    void statistics() {

        int total = static_cast<int>(books.size());
        int issued = 0;

        for (const Book& b : books) {

            if (b.issued)
                issued++;
        }

        cout << "\n========== LIBRARY STATISTICS ==========\n";

        cout << "Total Books      : " << total << "\n";
        cout << "Available Books  : " << total - issued << "\n";
        cout << "Issued Books     : " << issued << "\n";

        cout << "Hash Table Size  : "
             << hashTable.size() << "\n";

        cout << "Load Factor      : "
             << fixed << setprecision(2)
             << hashTable.loadFactor() << "\n";

        cout << "========================================\n";
    }

// =================================
// WEBSITE API FUNCTIONS
// =================================

// Return all books
vector<Book> getAllBooks() {
    vector<Book> result;

    for (const Book& book : books) {
        result.push_back(book);
    }

    return result;
}

// Find book by ID
bool getBookById(int id, Book& result) {
    Book* book = hashTable.search(id);

    if (!book) {
        return false;
    }

    result = *book;
    return true;
}

// Add book from website
bool addBookWeb(int id, string title,
                string author, string publisher) {

    if (id <= 0 || title.empty()) {
        return false;
    }

    if (hashTable.search(id)) {
        return false;
    }

    books.emplace_back(id, title, author, publisher);

    Book* book = &books.back();

    indexBook(book);
    saveToFile();

    return true;
}

// Update book from website
bool updateBookWeb(int id, string title,
                   string author, string publisher) {

    Book* book = hashTable.search(id);

    if (!book || title.empty()) {
        return false;
    }

    // Remove old title from BST
    bst.remove(book);

    // Update central record
    book->title = title;
    book->author = author;
    book->publisher = publisher;

    // Insert updated title
    bst.insert(book);

    saveToFile();

    return true;
}

// Delete book from website
bool deleteBookWeb(int id) {

    Book* book = hashTable.search(id);

    if (!book) {
        return false;
    }

    bst.remove(book);
    avl.remove(id);
    hashTable.remove(id);

    auto it = find_if(
        books.begin(),
        books.end(),
        [id](const Book& b) {
            return b.id == id;
        }
    );

    if (it != books.end()) {
        books.erase(it);
    }

    saveToFile();

    return true;
}

// Issue book from website
bool issueBookWeb(int id) {

    Book* book = hashTable.search(id);

    if (!book || book->issued) {
        return false;
    }

    book->issued = true;
    saveToFile();

    return true;
}

// Return book from website
bool returnBookWeb(int id) {

    Book* book = hashTable.search(id);

    if (!book || !book->issued) {
        return false;
    }

    book->issued = false;
    saveToFile();

    return true;
}

// Statistics for website
int getTotalBooks() {
    return static_cast<int>(books.size());
}

int getIssuedBooks() {
    int issued = 0;

    for (const Book& book : books) {
        if (book.issued) {
            issued++;
        }
    }

    return issued;
}

int getAvailableBooks() {
    return getTotalBooks() - getIssuedBooks();
}

int getHashTableSize() {
    return hashTable.size();
}

double getHashLoadFactor() {
    return hashTable.loadFactor();
}

    // Public save function
    void save() {
        saveToFile();
    }
};

#endif
