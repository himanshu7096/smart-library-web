
#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    string publisher;
    bool issued;

    Book(int i, string t, string a, string p) {
        id = i;
        title = t;
        author = a;
        publisher = p;
        issued = false;
    }
};

// Converts title into lowercase
inline string normalize(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });

    return s;
}

// Displays complete book information
inline void displayBook(Book* b) {

    if (b == nullptr)
        return;

    cout << "\n-----------------------------\n";

    cout << "Book ID   : " << b->id << "\n";
    cout << "Title     : " << b->title << "\n";
    cout << "Author    : " << b->author << "\n";
    cout << "Publisher : " << b->publisher << "\n";

    cout << "Status    : "
         << (b->issued ? "Issued" : "Available")
         << "\n";

    cout << "-----------------------------\n";
}

#endif
