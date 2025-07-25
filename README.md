# Distributed File Sharing System

A C++ server for uploading, downloading, and sharing files with other users. Built with **Poco C++** and **YugabyteDB**.

---

## Features

- Upload files via HTTP  
- Download files by filename  
- Share files with other users  
- List files shared with a user  
- Database-backed metadata using YugabyteDB (PostgreSQL-compatible protocol)

---

## Tech Stack

- **Language**: C++  
- **Web Framework**: Poco C++ Libraries  
- **Database**: YugabyteDB (running in Docker)  
- **Build Tool**: CMake

---

## Architecture

Modular design with the following components:

- `UploadHandler` – Stores uploaded files and inserts metadata into the database  
- `DownloadHandler` – Serves files by filename  
- `ShareHandler` – Allows sharing a file with another user  
- `SharedWithHandler` – Lists files shared with a user  
- `RequestHandlerFactory` – Routes incoming HTTP requests to the appropriate handler  
- `UserManager` – Manages users and basic user verification

---

## File Structure

project1/
├── .vscode/
│   ├── c_cpp_properties.json
│   ├── cpp_properties.json
│   └── settings.json
├── build/
├── file-sharing-system/
│   ├── CMakeLists.txt
│   ├── SAMPLE-PDF.pdf
│   └── src/
│       ├── main.cpp
│       ├── UserManager.cpp
│       ├── UserManager.h
│       ├── UploadHandler.cpp
│       ├── UploadHandler.h
│       ├── DownloadHandler.cpp
│       ├── DownloadHandler.h
│       ├── ShareHandler.cpp
│       ├── ShareHandler.h
│       ├── SharedWithHandler.cpp
│       ├── SharedWithHandler.h
│       ├── RequestHandler.cpp
│       ├── RequestHandler.h
│       ├── RequestHandlerFactory.cpp
│       └── RequestHandlerFactory.h
├── results/
├── test_uploads/
├── uploads/
│   ├── archive.zip
│   ├── document.pdf
│   ├── image.png
│   └── testfile.txt
├── CMakeLists.txt
├── download
├── myserver
├── README.md
└── test.sh


---

## Setup Instructions

### 1. Build

```bash
mkdir build && cd build
cmake ..
make
./file_sharing_server

### 2. Run YugabyteDB (in Docker)

docker run -d --name yugabytedb -p 5433:5433 -p 15433:15433 yugabytedb/yugabyte

Access YugabyteDB UI at: http://localhost:15433

### 3. Testing the System

-Ensure the server is running: ./file_sharing_server
-Make the test script executable: chmod +x test.sh
-Run: ./test.sh

## Future Improvements

-Add user authentication and session/token management.
-Implement expiring shared links or access controls.
-Fully Dockerize the server and DB together.
-Add nlohmann/json for structured JSON responses.

## Challenges Faced

-Getting POCO and libpq working on macOS M1
-Managing database schema consistency
-Ensuring cross-platform compatibility
