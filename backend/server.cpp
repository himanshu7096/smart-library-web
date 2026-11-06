#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include "Library.h"
#include "json.hpp"

using json = nlohmann::json;

Library library;

int main() {
    httplib::Server server;

    auto addCors = [](httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    server.set_post_routing_handler(
        [&](const httplib::Request&, httplib::Response& res) {
            addCors(res);
        }
    );

    server.Options(R"(/.*)", [&](const httplib::Request&, httplib::Response& res) {
        addCors(res);
        res.status = 204;
    });

    server.Get("/api/books", [](const httplib::Request&, httplib::Response& res) {
        json result = json::array();

        for (const Book& book : library.getAllBooks()) {
            result.push_back({
                {"id", book.id},
                {"title", book.title},
                {"author", book.author},
                {"publisher", book.publisher},
                {"issued", book.issued}
            });
        }

        res.set_content(result.dump(4), "application/json");
    });

    server.Get(R"(/api/books/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
            int id = std::stoi(req.matches[1].str());
            Book book(0, "", "", "");

            if (!library.getBookById(id, book)) {
                res.status = 404;
                res.set_content(
                    R"({"message":"Book not found"})",
                    "application/json"
                );
                return;
            }

            json result = {
                {"id", book.id},
                {"title", book.title},
                {"author", book.author},
                {"publisher", book.publisher},
                {"issued", book.issued}
            };

            res.set_content(result.dump(4), "application/json");
        }
    );

    server.Post("/api/books",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                json data = json::parse(req.body);

                bool success = library.addBookWeb(
                    data.at("id").get<int>(),
                    data.at("title").get<std::string>(),
                    data.at("author").get<std::string>(),
                    data.at("publisher").get<std::string>()
                );

                res.status = success ? 201 : 400;

                res.set_content(
                    json({
                        {"success", success},
                        {"message", success
                            ? "Book added successfully"
                            : "Invalid details or duplicate ID"}
                    }).dump(),
                    "application/json"
                );
            }
            catch (...) {
                res.status = 400;
                res.set_content(
                    R"({"success":false,"message":"Invalid JSON or missing fields"})",
                    "application/json"
                );
            }
        }
    );

    server.Put(R"(/api/books/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                int id = std::stoi(req.matches[1].str());
                json data = json::parse(req.body);

                bool success = library.updateBookWeb(
                    id,
                    data.at("title").get<std::string>(),
                    data.at("author").get<std::string>(),
                    data.at("publisher").get<std::string>()
                );

                res.status = success ? 200 : 400;

                res.set_content(
                    json({
                        {"success", success},
                        {"message", success
                            ? "Book updated successfully"
                            : "Book not found or invalid title"}
                    }).dump(),
                    "application/json"
                );
            }
            catch (...) {
                res.status = 400;
                res.set_content(
                    R"({"success":false,"message":"Invalid request"})",
                    "application/json"
                );
            }
        }
    );

    server.Delete(R"(/api/books/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
            int id = std::stoi(req.matches[1].str());
            bool success = library.deleteBookWeb(id);

            res.status = success ? 200 : 404;

            res.set_content(
                json({
                    {"success", success},
                    {"message", success
                        ? "Book deleted successfully"
                        : "Book not found"}
                }).dump(),
                "application/json"
            );
        }
    );

    server.Post(R"(/api/books/(\d+)/issue)",
        [](const httplib::Request& req, httplib::Response& res) {
            int id = std::stoi(req.matches[1].str());
            bool success = library.issueBookWeb(id);

            res.status = success ? 200 : 400;

            res.set_content(
                json({
                    {"success", success},
                    {"message", success
                        ? "Book issued successfully"
                        : "Book not found or already issued"}
                }).dump(),
                "application/json"
            );
        }
    );

    server.Post(R"(/api/books/(\d+)/return)",
        [](const httplib::Request& req, httplib::Response& res) {
            int id = std::stoi(req.matches[1].str());
            bool success = library.returnBookWeb(id);

            res.status = success ? 200 : 400;

            res.set_content(
                json({
                    {"success", success},
                    {"message", success
                        ? "Book returned successfully"
                        : "Book not found or already available"}
                }).dump(),
                "application/json"
            );
        }
    );

    server.Get("/api/statistics",
        [](const httplib::Request&, httplib::Response& res) {
            json result = {
                {"totalBooks", library.getTotalBooks()},
                {"issuedBooks", library.getIssuedBooks()},
                {"availableBooks", library.getAvailableBooks()},
                {"hashTableSize", library.getHashTableSize()},
                {"loadFactor", library.getHashLoadFactor()}
            };

            res.set_content(result.dump(4), "application/json");
        }
    );

    const char* frontendEnv = std::getenv("FRONTEND_DIR");
    std::string frontendDir;

    if (frontendEnv != nullptr) {
        frontendDir = frontendEnv;
    } else {
        frontendDir = "../frontend";
    }

    server.set_mount_point("/", frontendDir.c_str());

    const char* portEnv = std::getenv("PORT");
    int port = 8080;

    if (portEnv != nullptr) {
        try {
            port = std::stoi(portEnv);
        }
        catch (...) {
            port = 8080;
        }
    }

    std::cout << "Smart Library server running on port "
              << port << "\n";

    if (!server.listen("0.0.0.0", port)) {
        std::cerr << "Server failed to start.\n";
        return 1;
    }

    return 0;
}