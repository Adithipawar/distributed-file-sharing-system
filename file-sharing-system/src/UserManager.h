#pragma once
#include <string>
#include <unordered_map>
#include <mutex>

class UserManager {
public:
    bool registerUser(const std::string& username, const std::string& password);
    bool loginUser(const std::string& username, const std::string& password);

private:
    std::unordered_map<std::string, std::string> _users;
    std::mutex _mutex;
};
