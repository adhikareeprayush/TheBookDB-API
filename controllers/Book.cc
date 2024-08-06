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
    std::ofstream file(CSV_FILE, std::ios::app);

    for (const auto& book : books)
    {
        file << book.toCSV() << "\n";
    }
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
