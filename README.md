# TheBookDB

TheBookDB is a powerful API built using C++ and the Drogon framework. It provides information about over 11,000 books, sourced from a Kaggle CSV file.

## Features

- Retrieve book information
- Filter books by various criteria
- Add new books to the database
- Update existing book information
- Delete books from the database

## API Endpoints

- `GET /books`: Retrieve a list of books
- `GET /books/filter`: Filter books based on specific criteria
- `POST /books`: Add a new book
- `PATCH /books/{bookID}`: Update an existing book
- `DELETE /books/{bookID}`: Delete a book
- `PUT /books/{bookID}`: Update or add a book

## Technology Stack

- C++
- Drogon framework
- JsonCpp
- Linux (Ubuntu)

## Installation

Thank you for providing the additional dependency information. I'll update the README to include these dependencies. Here's the revised Installation section:

### Prerequisites

Ensure you have the following dependencies installed on your Ubuntu system:

1. Environment setup:

   ```
   sudo apt install git gcc g++ cmake
   ```

2. JsonCpp:

   ```
   sudo apt install libjsoncpp-dev
   ```

3. UUID:

   ```
   sudo apt install uuid-dev
   ```

4. zlib:

   ```
   sudo apt install zlib1g-dev
   ```

5. OpenSSL (Optional, for HTTPS support):

   ```
   sudo apt install openssl libssl-dev
   ```

### Building the Project

1. Clone the repository:

   ```
   git clone https://github.com/yourusername/TheBookDB.git
   ```

2. Navigate to the project directory:

   ```
   cd TheBookDB
   ```

3. Create a build directory and navigate to it:

   ```
   mkdir build && cd build
   ```

4. Generate the build files:

   ```
   cmake ..
   ```

5. Build the project:

   ```
   make
   ```

6. Run the server:

   ```
   ./MyDrogonAPI
   ```

The server should now be running and ready to accept requests.

## Usage

Once the server is running, you can make HTTP requests to the API endpoints. Here are some examples:

- Get all books:

  ```
  GET http://localhost:8080/books
  ```

- Filter books:

  ```
  GET http://localhost:8080/books/filter?startDate=01/01/2000&endDate=12/31/2020
  ```

- Add a new book:

  ```
  POST http://localhost:8080/books
  Content-Type: application/json

  {
    "title": "New Book",
    "authors": "John Doe",
    "publicationDate": "05/15/2023"
  }
  ```

- Update a new existing (Patch):

  ```
  Patch http://localhost:8080/books/{bookID}
  Content-Type: application/json

  {
    "title": "New Book title",
    "authors": "John Doe",
    "publicationDate": "05/15/2023"
  }
  ```

- Add or update a book (PUT):

  ```
  PUT http://localhost:8080/books/{bookID}
  Content-Type: application/json

  {
    "title": "Edited or Added Title",
    "authors": "John Doe",
    "publicationDate": "05/15/2023"
  }
  ```

- Delete a book:

  ```
  DELETE http://localhost:8080/books/{bookID}
  ```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
