#include "Book.h"
#include <fstream>
#include <jsoncpp/json/json.h>

const std::string BookController::CSV_FILE = "books.csv";

std::vector<Book> BookController::readBooksFromCSV(int limit, int offset) {
    std::vector<Book> books;
    std::ifstream file(CSV_FILE);
    std::string line;
    
    // Skip header
    std::getline(file, line);

    int count = 0;
    while (std::getline(file, line) && (limit == -1 || books.size() < limit)) {
        if (count >= offset) {
            books.push_back(Book::fromCSV(line));
        }
        count++;
    }
    return books;
}

void BookController::writeBooksToCSV(const std::vector<Book>& books) {
    std::ofstream file(CSV_FILE);
    file << "title,authors,description,category,publisher,price,publish_date_month,publish_date_day,publish_date_year\n";
    for (const auto& book : books) {
        file << book.toCSV() << "\n";
    }
}

void BookController::addBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("Invalid JSON in request body");
        callback(resp);
        return;
    }

    Book newBook;
    newBook.title = (*json)["title"].asString();
    newBook.authors = (*json)["authors"].asString();
    newBook.description = (*json)["description"].asString();
    newBook.category = (*json)["category"].asString();
    newBook.publisher = (*json)["publisher"].asString();
    newBook.price = (*json)["price"].asString();
    newBook.publish_date_month = (*json)["publish_date_month"].asString();
    newBook.publish_date_day = (*json)["publish_date_day"].asString();
    newBook.publish_date_year = (*json)["publish_date_year"].asString();

    // Read existing books
    auto books = readBooksFromCSV();

    // Add new book
    books.push_back(newBook);

    // Write updated books list to CSV
    writeBooksToCSV(books);

    // Prepare response
    Json::Value responseJson;
    responseJson["title"] = newBook.title;
    responseJson["authors"] = newBook.authors;
    responseJson["description"] = newBook.description;
    responseJson["category"] = newBook.category;
    responseJson["publisher"] = newBook.publisher;
    responseJson["price"] = newBook.price;
    responseJson["publish_date_month"] = newBook.publish_date_month;
    responseJson["publish_date_day"] = newBook.publish_date_day;
    responseJson["publish_date_year"] = newBook.publish_date_year;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(responseJson);
    resp->setStatusCode(drogon::k201Created);
    callback(resp);
}

void BookController::deleteBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title)
{
    // Read existing books
    auto books = readBooksFromCSV();

    // Find the book to delete
    auto it = std::find_if(books.begin(), books.end(), [&title](const Book& book) {
        return book.title == title;
    });

    if (it == books.end()) {
        // Book not found
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k404NotFound);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("Book not found");
        callback(resp);
        return;
    }

    // Remove the book
    books.erase(it);

    // Write updated books list to CSV
    writeBooksToCSV(books);

    // Prepare response
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
    resp->setBody("Book deleted successfully");
    callback(resp);
}

void BookController::updateBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("Invalid JSON in request body");
        callback(resp);
        return;
    }

    // Read existing books
    auto books = readBooksFromCSV();

    // Find the book to update
    auto it = std::find_if(books.begin(), books.end(), [&title](const Book& book) {
        return book.title == title;
    });

    bool isNewBook = (it == books.end());
    Book& book = isNewBook ? books.emplace_back() : *it;

    // Update or create the book
    book.title = title;  // Use the title from the URL
    book.authors = json->get("authors", book.authors).asString();
    book.description = json->get("description", book.description).asString();
    book.category = json->get("category", book.category).asString();
    book.publisher = json->get("publisher", book.publisher).asString();
    book.price = json->get("price", book.price).asString();
    book.publish_date_month = json->get("publish_date_month", book.publish_date_month).asString();
    book.publish_date_day = json->get("publish_date_day", book.publish_date_day).asString();
    book.publish_date_year = json->get("publish_date_year", book.publish_date_year).asString();

    // Write updated books list to CSV
    writeBooksToCSV(books);

    // Prepare response
    Json::Value responseJson;
    responseJson["title"] = book.title;
    responseJson["authors"] = book.authors;
    responseJson["description"] = book.description;
    responseJson["category"] = book.category;
    responseJson["publisher"] = book.publisher;
    responseJson["price"] = book.price;
    responseJson["publish_date_month"] = book.publish_date_month;
    responseJson["publish_date_day"] = book.publish_date_day;
    responseJson["publish_date_year"] = book.publish_date_year;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(responseJson);
    resp->setStatusCode(isNewBook ? drogon::k201Created : drogon::k200OK);
    callback(resp);
}


void BookController::patchBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("Invalid JSON in request body");
        callback(resp);
        return;
    }

    // Read existing books
    auto books = readBooksFromCSV();

    // Find the book to update
    auto it = std::find_if(books.begin(), books.end(), [&title](const Book& book) {
        return book.title == title;
    });

    if (it == books.end()) {
        // Book not found
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k404NotFound);
        resp->setContentTypeCode(drogon::CT_TEXT_PLAIN);
        resp->setBody("Book not found");
        callback(resp);
        return;
    }

    // Update only the fields provided in the JSON
    if (json->isMember("authors")) it->authors = (*json)["authors"].asString();
    if (json->isMember("description")) it->description = (*json)["description"].asString();
    if (json->isMember("category")) it->category = (*json)["category"].asString();
    if (json->isMember("publisher")) it->publisher = (*json)["publisher"].asString();
    if (json->isMember("price")) it->price = (*json)["price"].asString();
    if (json->isMember("publish_date_month")) it->publish_date_month = (*json)["publish_date_month"].asString();
    if (json->isMember("publish_date_day")) it->publish_date_day = (*json)["publish_date_day"].asString();
    if (json->isMember("publish_date_year")) it->publish_date_year = (*json)["publish_date_year"].asString();

    // Write updated books list to CSV
    writeBooksToCSV(books);

    // Prepare response
    Json::Value responseJson;
    responseJson["title"] = it->title;
    responseJson["authors"] = it->authors;
    responseJson["description"] = it->description;
    responseJson["category"] = it->category;
    responseJson["publisher"] = it->publisher;
    responseJson["price"] = it->price;
    responseJson["publish_date_month"] = it->publish_date_month;
    responseJson["publish_date_day"] = it->publish_date_day;
    responseJson["publish_date_year"] = it->publish_date_year;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(responseJson);
    resp->setStatusCode(drogon::k200OK);
    callback(resp);
}

Book BookController::findBookByTitle(const std::string& title) {
    std::ifstream file(CSV_FILE);
    std::string line;
    
    // Skip header
    std::getline(file, line);

    while (std::getline(file, line)) {
        Book book = Book::fromCSV(line);
        if (book.title == title) {
            return book;
        }
    }
    throw std::runtime_error("Book not found");
}

void BookController::getBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    // Retrieve parameters from the request
    std::string title = req->getParameter("title");
    std::string authors = req->getParameter("authors");
    std::string category = req->getParameter("category");
    std::string publisher = req->getParameter("publisher");
    std::string price = req->getParameter("price");
    std::string publish_date_month = req->getParameter("publish_date_month");
    std::string publish_date_day = req->getParameter("publish_date_day");
    std::string publish_date_year = req->getParameter("publish_date_year");

    // Parse pagination parameters
    std::string pageStr = req->getParameter("page");
    std::string pageSizeStr = req->getParameter("pageSize");

    int page = pageStr.empty() ? 1 : std::stoi(pageStr);
    int pageSize = pageSizeStr.empty() ? 20 : std::stoi(pageSizeStr);
    int offset = (page - 1) * pageSize;

    // Query books based on parameters
    std::vector<Book> filteredBooks;
    std::vector<Book> allBooks = readBooksFromCSV(-1, 0); // Read all books

    //if no filters are provided, return all books
    if (title.empty() && authors.empty() && category.empty() && publisher.empty() && price.empty() && publish_date_month.empty() && publish_date_day.empty() && publish_date_year.empty()) {
        for (const auto& book : allBooks) {
            filteredBooks.push_back(book);
        }
    }


    for (const auto& book : allBooks) {
        // Apply filters
        if ((!title.empty() && book.title == title) ||
            (!authors.empty() && book.authors.find(authors) != std::string::npos) ||
            (!category.empty() && book.category == category) ||
            (!publisher.empty() && book.publisher == publisher) ||
            (!price.empty() && book.price == price) ||
            (!publish_date_month.empty() && book.publish_date_month == publish_date_month) ||
            (!publish_date_day.empty() && book.publish_date_day == publish_date_day) ||
            (!publish_date_year.empty() && book.publish_date_year == publish_date_year)) {
            filteredBooks.push_back(book);
        }
    }

    // Apply pagination
    std::vector<Book> pagedBooks;
    for (size_t i = offset; i < filteredBooks.size() && pagedBooks.size() < pageSize; ++i) {
        pagedBooks.push_back(filteredBooks[i]);
    }

    // Convert to JSON response
    Json::Value ret(Json::arrayValue);
    for (const auto& book : pagedBooks) {
        Json::Value bookJson;
        bookJson["title"] = book.title;
        bookJson["authors"] = book.authors;
        bookJson["description"] = book.description;
        bookJson["category"] = book.category;
        bookJson["publisher"] = book.publisher;
        bookJson["price"] = book.price;
        bookJson["publish_date_month"] = book.publish_date_month;
        bookJson["publish_date_day"] = book.publish_date_day;
        bookJson["publish_date_year"] = book.publish_date_year;

        ret.append(bookJson);
    }

    // Create HTTP response
    auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void BookController::getBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, std::string title) {
    try {
        Book book = findBookByTitle(title);
        Json::Value ret;
        ret["title"] = book.title;
        ret["authors"] = book.authors;
        ret["description"] = book.description;
        ret["category"] = book.category;
        ret["publisher"] = book.publisher;
        ret["price"] = book.price;
        ret["publish_date_month"] = book.publish_date_month;
        ret["publish_date_day"] = book.publish_date_day;
        ret["publish_date_year"] = book.publish_date_year;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(ret);
        callback(resp);
    } catch (const std::runtime_error&) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k404NotFound);
        callback(resp);
    }
}