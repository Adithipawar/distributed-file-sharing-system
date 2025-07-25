cmake_minimum_required(VERSION 3.10)
project(file_sharing_server)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set path for Poco (macOS/Homebrew)
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/poco")

# Find Poco and PostgreSQL
find_package(Poco REQUIRED COMPONENTS Net Util)
find_package(PostgreSQL REQUIRED)

# Include headers
include_directories(
    ${Poco_INCLUDE_DIRS}
    ${PostgreSQL_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

# ✅ Grab all .cpp files from src/
file(GLOB SOURCES "src/*.cpp")

# Build the executable
add_executable(file_sharing_server ${SOURCES})

# Link libraries
target_link_libraries(file_sharing_server
    ${Poco_LIBRARIES}
    ${PostgreSQL_LIBRARIES}
)

message(STATUS "Source files: ${SOURCES}")
