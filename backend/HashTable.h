
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "Book.h"
#include <vector>
#include <list>

class HashTable {
private:

    // Hash table using Separate Chaining
    vector<list<Book*>> table;

    int count;

    // Hash function
    int hashFunction(int id) {
        return id % static_cast<int>(table.size());
    }

    // Dynamic resizing
    void rehash() {

        vector<list<Book*>> oldTable;

        oldTable.swap(table);

        // Double the table size
        table.resize(oldTable.size() * 2);

        count = 0;

        // Reinsert all existing books
        for (auto& bucket : oldTable) {

            for (Book* book : bucket) {

                insert(book);
            }
        }
    }

public:

    // Constructor
    HashTable() {

        table.resize(11);

        count = 0;
    }

    // Insert book
    void insert(Book* book) {

        if (search(book->id))
            return;

        // Check load factor
        if (static_cast<double>(count + 1) /
            table.size() > 0.75) {

            rehash();
        }

        int index = hashFunction(book->id);

        // Insert using Separate Chaining
        table[index].push_back(book);

        count++;
    }

    // Search book by ID
    Book* search(int id) {

        int index = hashFunction(id);

        for (Book* book : table[index]) {

            if (book->id == id) {

                return book;
            }
        }

        return nullptr;
    }

    // Delete book by ID
    void remove(int id) {

        int index = hashFunction(id);

        for (auto it = table[index].begin();
             it != table[index].end(); ++it) {

            if ((*it)->id == id) {

                table[index].erase(it);

                count--;

                return;
            }
        }
    }

    // Calculate load factor
    double loadFactor() {

        return static_cast<double>(count) /
               table.size();
    }

    // Total books stored
    int size() {

        return count;
    }

};

#endif
