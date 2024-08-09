#include <drogon/HttpController.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>

struct Book {
    std::string bookID;
    std::string title;
    std::string authors;
    std::string avgRating;
    std::string isbn;
    std::string isbn13;
    std::string languageCode;
    std::string numPages;
    std::string ratingsCount;
    std::string textReviewsCount;
    std::string publicationDate;
    std::string publisher;

    static std::string escapeCSV(const std::string& str);
    std::string toCSV() const;
    static Book fromCSV(const std::string& line);
};

class BookController : public drogon::HttpController<BookController>
{
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BookController::getBooks, "/books", drogon::Get);
    ADD_METHOD_TO(BookController::filterBooks, "/books/filter", drogon::Get);
    ADD_METHOD_TO(BookController::addBook, "/books", drogon::Post);
    ADD_METHOD_TO(BookController::updateBook, "/books/{bookID}", drogon::Patch);
    ADD_METHOD_TO(BookController::deleteBook, "/books/{bookID}", drogon::Delete);
    ADD_METHOD_TO(BookController::putBook, "/books/{bookID}", drogon::Put);
    METHOD_LIST_END

    void getBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void filterBooks(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void addBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void updateBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void deleteBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void putBook(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    static const std::string CSV_FILE;
    static std::string escapeCSV(const std::string& str);
    static std::vector<Book> readBooksFromCSV(int limit = -1, int offset = 0);
    static void writeBooksToCSV(const std::vector<Book>& books);
    static Book findBookByTitle(const std::string& title);
    static bool dateInRange(const std::string& date, const std::string& startDate, const std::string& endDate);
    static bool bookExists(const std::string& bookID);
};