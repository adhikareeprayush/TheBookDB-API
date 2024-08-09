#include "Book.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <jsoncpp/json/json.h>
#include <algorithm>

// CSV file path
const std::string BookController::CSV_FILE = "books.csv";

// Utility function to escape CSV strings
std::string Book::escapeCSV(const std::string& str)
{
    std::string escapedStr = str;
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos)
    {
        escapedStr = '"' + str + '"';
    }
    return escapedStr;
}

// Convert Book object to CSV format
std::string Book::toCSV() const
{
    std::ostringstream oss;
    oss << escapeCSV(bookID) << ','
        << escapeCSV(title) << ','
        << escapeCSV(authors) << ','
        << escapeCSV(avgRating) << ','
        << escapeCSV(isbn) << ','
        << escapeCSV(isbn13) << ','
        << escapeCSV(languageCode) << ','
        << escapeCSV(numPages) << ','
        << escapeCSV(ratingsCount) << ','
        << escapeCSV(textReviewsCount) << ','
        << escapeCSV(publicationDate) << ','
        << escapeCSV(publisher);
    return oss.str();
}

// Create Book object from CSV line
Book Book::fromCSV(const std::string& line)
{
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, ','))
    {
        // Remove leading and trailing spaces from each token
        token.erase(0, token.find_first_not_of(' '));
        token.erase(token.find_last_not_of(' ') + 1);
        tokens.push_back(token);
    }

    Book book;
    if (tokens.size() >= 12)
    {
        book.bookID = tokens[0];
        book.title = tokens[1];
        book.authors = tokens[2];
        book.avgRating = tokens[3];
        book.isbn = tokens[4];
        book.isbn13 = tokens[5];
        book.languageCode = tokens[6];
        book.numPages = tokens[7];
        book.ratingsCount = tokens[8];
        book.textReviewsCount = tokens[9];
        book.publicationDate = tokens[10];
        book.publisher = tokens[11];
    }

    return book;
}

// Read books from CSV file
std::vector<Book> BookController::readBooksFromCSV(int limit, int offset)
{
    std::ifstream file(CSV_FILE);
    std::vector<Book> books;
    std::string line;
    int count = 0;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line))
    {
        if (count >= offset && (limit == -1 || count < offset + limit))
        {
            books.push_back(Book::fromCSV(line));
        }
        count++;
    }

    return books;
}

// Write books to CSV file
void BookController::writeBooksToCSV(const std::vector<Book>& books)
{
    std::ofstream file(CSV_FILE);

    if (!file.is_open())
    {
        throw std::runtime_error("Unable to open CSV file for writing");
    }

    // Write header
    file << "bookID,title,authors,avgRating,isbn,isbn13,languageCode,numPages,ratingsCount,textReviewsCount,publicationDate,publisher\n";

    // Write book data
    for (const auto& book : books)
    {
        file << book.toCSV() << "\n";
    }

    file.close();
}

// Find book by title
Book BookController::findBookByTitle(const std::string& title)
{
    std::ifstream file(CSV_FILE);
    std::string line;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line))
    {
        Book book = Book::fromCSV(line);
        if (book.title == title)
        {
            return book;
        }
    }

    throw std::runtime_error("Book not found");
}

// Utility function to parse date strings
std::tm parseDate(const std::string& date)
{
    std::tm tm = {};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%m/%d/%Y");
    if (ss.fail())
    {
        throw std::runtime_error("Failed to parse date: " + date);
    }
    return tm;
}

// Check if a date is in the specified range
bool BookController::dateInRange(const std::string& date, const std::string& startDate, const std::string& endDate)
{
    try
    {
        std::tm dateTm = parseDate(date);
        std::tm startTm = parseDate(startDate);
        std::tm endTm = parseDate(endDate);

        std::time_t dateTime = std::mktime(&dateTm);
        std::time_t startDateTime = std::mktime(&startTm);
        std::time_t endDateTime = std::mktime(&endTm);

        return dateTime >= startDateTime && dateTime <= endDateTime;
    }
    catch (const std::runtime_error& e)
    {
        // Log or handle parsing errors
        std::cerr << e.what() << std::endl;
        return false;  // or true depending on how you want to handle parsing failures
    }
}

// Handler for the getBooks endpoint
void BookController::getBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto queryParams = req->getParameters();
    int limit = -1;
    int offset = 0;

    if (queryParams.find("limit") != queryParams.end())
    {
        limit = std::stoi(queryParams.at("limit"));
    }

    if (queryParams.find("offset") != queryParams.end())
    {
        offset = std::stoi(queryParams.at("offset"));
    }

    std::string bookID;
    std::string title;
    std::string authors;
    std::string avgRating;
    std::string numPages;
    std::string publicationDate;
    std::string publisher;
    std::string isbn;
    std::string isbn13;
    std::string languageCode;


    if (queryParams.find("bookID") != queryParams.end())
    {
        bookID = queryParams.at("bookID");
    }
    if (queryParams.find("authors") != queryParams.end())
    {
        authors = queryParams.at("authors");
    }
    if (queryParams.find("title") != queryParams.end())
    {
        title = queryParams.at("title");
    }
    if (queryParams.find("publisher") != queryParams.end())
    {
        publisher = queryParams.at("publisher");
    }
    if (queryParams.find("publicationDate") != queryParams.end())
    {
        publicationDate = queryParams.at("publicationDate");
    }
    if (queryParams.find("numPages") != queryParams.end())
    {
        numPages = queryParams.at("numPages");
    }
    if (queryParams.find("avgRating") != queryParams.end())
    {
        avgRating = queryParams.at("avgRating");
    }
    if (queryParams.find("isbn") != queryParams.end())
    {
        isbn = queryParams.at("isbn");
    }
    if (queryParams.find("isbn13") != queryParams.end())
    {
        isbn13 = queryParams.at("isbn13");
    }
    if (queryParams.find("languageCode") != queryParams.end())
    {
        languageCode = queryParams.at("languageCode");
    }


    try
    {
        auto books = readBooksFromCSV(limit, offset);
        Json::Value jsonBooks(Json::arrayValue);

        for (const auto& book : books)
        {
            bool matches = true;

            if (!bookID.empty() && book.bookID != bookID)
            {
                matches = false;
            }
            if (!title.empty() && book.title != title)
            {
                matches = false;
            }
            if (!authors.empty() && book.authors != authors)
            {
                matches = false;
            }
            if (!avgRating.empty() && book.avgRating != avgRating)
            {
                matches = false;
            }
            if (!isbn.empty() && book.isbn != isbn)
            {
                matches = false;
            }
            if (!isbn13.empty() && book.isbn13 != isbn13)
            {
                matches = false;
            }
            if (!languageCode.empty() && book.languageCode != languageCode)
            {
                matches = false;
            }
            if (!numPages.empty() && book.numPages != numPages)
            {
                matches = false;
            }
            if (!publisher.empty() && book.publisher != publisher)
            {
                matches = false;
            }
            if (!publicationDate.empty() && book.publicationDate != publicationDate)
            {
                matches = false;
            }


            if (matches)
            {
                Json::Value jsonBook;
                jsonBook["bookID"] = book.bookID;
                jsonBook["title"] = book.title;
                jsonBook["authors"] = book.authors;
                jsonBook["avgRating"] = book.avgRating;
                jsonBook["isbn"] = book.isbn;
                jsonBook["isbn13"] = book.isbn13;
                jsonBook["languageCode"] = book.languageCode;
                jsonBook["numPages"] = book.numPages;
                jsonBook["ratingsCount"] = book.ratingsCount;
                jsonBook["textReviewsCount"] = book.textReviewsCount;
                jsonBook["publicationDate"] = book.publicationDate;
                jsonBook["publisher"] = book.publisher;
                jsonBooks.append(jsonBook);
            }
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonBooks);
        callback(resp);
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody(e.what());
        callback(resp);
    }
}

// Handler for the filterBooks endpoint
void BookController::filterBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto queryParams = req->getParameters();
    std::string startDate;
    std::string endDate;
    std::string sortOrder = "ASC"; // Default to ascending

    if (queryParams.find("startDate") != queryParams.end())
    {
        startDate = queryParams.at("startDate");
    }

    if (queryParams.find("endDate") != queryParams.end())
    {
        endDate = queryParams.at("endDate");
    }

    if (queryParams.find("sortOrder") != queryParams.end())
    {
        sortOrder = queryParams.at("sortOrder");
    }

    try
    {
        auto books = readBooksFromCSV(); // Read all books
        Json::Value jsonBooks(Json::arrayValue);

        for (const auto& book : books)
        {
            bool matches = true;

            if (!startDate.empty() && !endDate.empty() && !dateInRange(book.publicationDate, startDate, endDate))
            {
                matches = false;
            }

            if (matches)
            {
                Json::Value jsonBook;
                jsonBook["bookID"] = book.bookID;
                jsonBook["title"] = book.title;
                jsonBook["authors"] = book.authors;
                jsonBook["avgRating"] = book.avgRating;
                jsonBook["isbn"] = book.isbn;
                jsonBook["isbn13"] = book.isbn13;
                jsonBook["languageCode"] = book.languageCode;
                jsonBook["numPages"] = book.numPages;
                jsonBook["ratingsCount"] = book.ratingsCount;
                jsonBook["textReviewsCount"] = book.textReviewsCount;
                jsonBook["publicationDate"] = book.publicationDate;
                jsonBook["publisher"] = book.publisher;
                jsonBooks.append(jsonBook);
            }
        }

        // Sort books by publication date
        std::vector<Json::Value> sortedBooks(jsonBooks.begin(), jsonBooks.end());
        std::sort(sortedBooks.begin(), sortedBooks.end(), [sortOrder](const Json::Value& a, const Json::Value& b) {
            std::tm aTm = {};
            std::tm bTm = {};
            std::istringstream ssA(a["publicationDate"].asString());
            std::istringstream ssB(b["publicationDate"].asString());
            ssA >> std::get_time(&aTm, "%m/%d/%Y");
            ssB >> std::get_time(&bTm, "%m/%d/%Y");
            std::time_t aTime = std::mktime(&aTm);
            std::time_t bTime = std::mktime(&bTm);

            return (sortOrder == "ASC") ? (aTime < bTime) : (aTime > bTime);
        });

        Json::Value jsonSortedBooks(Json::arrayValue);
        for (const auto& book : sortedBooks)
        {
            jsonSortedBooks.append(book);
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(jsonSortedBooks);
        callback(resp);
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setBody(e.what());
        callback(resp);
    }
}

// Handler for the addBook endpoint
void BookController::addBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid JSON format");
        callback(resp);
        return;
    }

    try
    {
        // Read existing books
        auto books = readBooksFromCSV();

        // Determine the new bookID
        int newID = 1;
        if (!books.empty())
        {
            auto lastBook = std::max_element(books.begin(), books.end(), [](const Book& a, const Book& b) {
                return std::stoi(a.bookID) < std::stoi(b.bookID);
            });
            newID = std::stoi(lastBook->bookID) + 1;
        }
        std::string newBookID = std::to_string(newID);

        // Create new book with the unique bookID
        Book book;
        book.bookID = newBookID;
        book.title = json->get("title", "").asString();
        book.authors = json->get("authors", "").asString();
        book.avgRating = json->get("avgRating", "").asString();
        book.isbn = json->get("isbn", "").asString();
        book.isbn13 = json->get("isbn13", "").asString();
        book.languageCode = json->get("languageCode", "").asString();
        book.numPages = json->get("numPages", "").asString();
        book.ratingsCount = json->get("ratingsCount", "").asString();
        book.textReviewsCount = json->get("textReviewsCount", "").asString();
        book.publicationDate = json->get("publicationDate", "").asString();
        book.publisher = json->get("publisher", "").asString();

        // Add the new book to the end of the list
        books.push_back(book);

        // Write the updated books list to the CSV file
        writeBooksToCSV(books);

        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->setBody("Book added successfully with ID: " + newBookID);
        callback(resp);
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody(e.what());
        callback(resp);
    }
}

// Handler for the updateBook endpoint
void BookController::updateBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid JSON format");
        callback(resp);
        return;
    }

    // Extract bookID from the URL path
    std::string path = req->getPath();
    std::string bookID = path.substr(path.find_last_of('/') + 1);

    if (bookID.empty())
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Book ID is required");
        callback(resp);
        return;
    }

    try
    {
        // Read existing books
        auto books = readBooksFromCSV();

        // Find the book to update
        auto it = std::find_if(books.begin(), books.end(), [&bookID](const Book& book) {
            return book.bookID == bookID;
        });

        if (it != books.end())
        {
            // Update the book details with the provided data
            if (json->isMember("title")) it->title = json->get("title", "").asString();
            if (json->isMember("authors")) it->authors = json->get("authors", "").asString();
            if (json->isMember("avgRating")) it->avgRating = json->get("avgRating", "").asString();
            if (json->isMember("isbn")) it->isbn = json->get("isbn", "").asString();
            if (json->isMember("isbn13")) it->isbn13 = json->get("isbn13", "").asString();
            if (json->isMember("languageCode")) it->languageCode = json->get("languageCode", "").asString();
            if (json->isMember("numPages")) it->numPages = json->get("numPages", "").asString();
            if (json->isMember("ratingsCount")) it->ratingsCount = json->get("ratingsCount", "").asString();
            if (json->isMember("textReviewsCount")) it->textReviewsCount = json->get("textReviewsCount", "").asString();
            if (json->isMember("publicationDate")) it->publicationDate = json->get("publicationDate", "").asString();
            if (json->isMember("publisher")) it->publisher = json->get("publisher", "").asString();

            // Write updated books list to CSV
            writeBooksToCSV(books);

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k200OK);
            resp->setBody("Book updated successfully");
            callback(resp);
        }
        else
        {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k404NotFound);
            resp->setBody("Book not found");
            callback(resp);
        }
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody(e.what());
        callback(resp);
    }
}

// Check if a book with the given ID exists
bool BookController::bookExists(const std::string& bookID)
{
    auto books = readBooksFromCSV();
    return std::any_of(books.begin(), books.end(), [&bookID](const Book& book) {
        return book.bookID == bookID;
    });
}

// Handler for the deleteBook endpoint
void BookController::deleteBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    // Extract bookID from the URL path
    std::string path = req->getPath();
    std::string bookID = path.substr(path.find_last_of('/') + 1);

    if (bookID.empty())
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Book ID is required");
        callback(resp);
        return;
    }

    try
    {
        // Read existing books
        auto books = readBooksFromCSV();

        // Find and remove the book with the given bookID
        auto it = std::remove_if(books.begin(), books.end(), [&bookID](const Book& book) {
            return book.bookID == bookID;
        });

        if (it != books.end())
        {
            // Erase the removed book from the vector
            books.erase(it, books.end());

            // Write updated books list to CSV
            writeBooksToCSV(books);

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k200OK);
            resp->setBody("Book deleted successfully");
            callback(resp);
        }
        else
        {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k404NotFound);
            resp->setBody("Book not found");
            callback(resp);
        }
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody(e.what());
        callback(resp);
    }
}

// Handler for the putBook endpoint
void BookController::putBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Invalid JSON format");
        callback(resp);
        return;
    }

    // Extract bookID from the URL path
    std::string path = req->getPath();
    std::string bookID = path.substr(path.find_last_of('/') + 1);

    if (bookID.empty())
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody("Book ID is required");
        callback(resp);
        return;
    }

    try
    {
        // Read existing books
        auto books = readBooksFromCSV();

        // Find the book to update
        auto it = std::find_if(books.begin(), books.end(), [&bookID](const Book& book) {
            return book.bookID == bookID;
        });

        if (it != books.end())
        {
            // Update the book details with the provided data
            if (json->isMember("title")) it->title = json->get("title", "").asString();
            if (json->isMember("authors")) it->authors = json->get("authors", "").asString();
            if (json->isMember("avgRating")) it->avgRating = json->get("avgRating", "").asString();
            if (json->isMember("isbn")) it->isbn = json->get("isbn", "").asString();
            if (json->isMember("isbn13")) it->isbn13 = json->get("isbn13", "").asString();
            if (json->isMember("languageCode")) it->languageCode = json->get("languageCode", "").asString();
            if (json->isMember("numPages")) it->numPages = json->get("numPages", "").asString();
            if (json->isMember("ratingsCount")) it->ratingsCount = json->get("ratingsCount", "").asString();
            if (json->isMember("textReviewsCount")) it->textReviewsCount = json->get("textReviewsCount", "").asString();
            if (json->isMember("publicationDate")) it->publicationDate = json->get("publicationDate", "").asString();
            if (json->isMember("publisher")) it->publisher = json->get("publisher", "").asString();
        }
        else
        {
            // Create a new book if it does not exist
            Book newBook;
            newBook.bookID = bookID;
            if (json->isMember("title")) newBook.title = json->get("title", "").asString();
            if (json->isMember("authors")) newBook.authors = json->get("authors", "").asString();
            if (json->isMember("avgRating")) newBook.avgRating = json->get("avgRating", "").asString();
            if (json->isMember("isbn")) newBook.isbn = json->get("isbn", "").asString();
            if (json->isMember("isbn13")) newBook.isbn13 = json->get("isbn13", "").asString();
            if (json->isMember("languageCode")) newBook.languageCode = json->get("languageCode", "").asString();
            if (json->isMember("numPages")) newBook.numPages = json->get("numPages", "").asString();
            if (json->isMember("ratingsCount")) newBook.ratingsCount = json->get("ratingsCount", "").asString();
            if (json->isMember("textReviewsCount")) newBook.textReviewsCount = json->get("textReviewsCount", "").asString();
            if (json->isMember("publicationDate")) newBook.publicationDate = json->get("publicationDate", "").asString();
            if (json->isMember("publisher")) newBook.publisher = json->get("publisher", "").asString();

            books.push_back(newBook);
        }

        // Write updated books list to CSV
        writeBooksToCSV(books);

        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k200OK);
        resp->setBody("Book updated or added successfully");
        callback(resp);
    }
    catch (const std::exception& e)
    {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setStatusCode(drogon::k400BadRequest);
        resp->setBody(e.what());
        callback(resp);
    }
}
