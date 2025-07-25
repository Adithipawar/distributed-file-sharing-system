#include "UserManager.h"

bool UserManager::registerUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_users.find(username) != _users.end()) return false;
    _users[username] = password;
    return true;
}

bool UserManager::loginUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = _users.find(username);
    return it != _users.end() && it->second == password;
}
