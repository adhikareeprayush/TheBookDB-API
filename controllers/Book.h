#pragma once

#include <drogon/HttpController.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>


struct Book {
    std::string title;
    std::string authors;
    std::string description;
    std::string category;
    std::string publisher;
    std::string price;
    std::string publish_date_month;
    std::string publish_date_day;
    std::string publish_date_year;
    

    static std::string escapeCSV(const std::string& str) {
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos) {
        std::string result = str;
        std::string::size_type pos = 0;
        while ((pos = result.find('"', pos)) != std::string::npos) {
            result.replace(pos, 1, "\"\"");
            pos += 2;
        }
        return "\"" + result + "\"";
    }
    return str;
    }

    std::string toCSV() const {
        return escapeCSV(title) + "," + 
               escapeCSV(authors) + "," + 
               escapeCSV(description) + "," + 
               escapeCSV(category) + "," + 
               escapeCSV(publisher) + "," + 
               escapeCSV(price) + "," +
                escapeCSV(publish_date_month) + "," +
                escapeCSV(publish_date_day) + "," +
                escapeCSV(publish_date_year);

    }

    static Book fromCSV(const std::string& line) {
        Book book;
        std::istringstream ss(line);
        std::string field;

        auto getNextField = [&ss]() {
            std::string field;
            if (ss.peek() == '"') {
                ss.ignore();
                std::getline(ss, field, '"');
                ss.ignore(2);
            } else {
                std::getline(ss, field, ',');
            }
            return field;
        };

        book.title = getNextField();
        book.authors = getNextField();
        book.description = getNextField();
        book.category = getNextField();
        book.publisher = getNextField();
        book.price = getNextField();
        book.publish_date_month = getNextField();
        book.publish_date_day = getNextField();
        book.publish_date_year = getNextField();

        return book;
    }
};

class BookController : public drogon::HttpController<BookController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BookController::getBooks, "/books", drogon::Get);
    ADD_METHOD_TO(BookController::getBook, "/books/{title}", drogon::Get);
    ADD_METHOD_TO(BookController::addBook, "/books", drogon::Post);
    ADD_METHOD_TO(BookController::deleteBook, "/books/{title}", drogon::Delete);
    ADD_METHOD_TO(BookController::updateBook, "/books/{title}", drogon::Put);
    ADD_METHOD_TO(BookController::patchBook, "/books/{title}", drogon::Patch);
    METHOD_LIST_END

    void patchBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title);
    void updateBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title);
    void deleteBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title);
    void addBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title);

private:
    static const std::string CSV_FILE;
    static void writeBooksToCSV(const std::vector<Book>& books);
    static std::vector<Book> readBooksFromCSV(int limit = -1, int offset = 0);
    static Book findBookByTitle(const std::string& title);
};