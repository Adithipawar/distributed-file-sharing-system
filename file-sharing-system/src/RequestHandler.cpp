#include "RequestHandler.h"
#include "UploadHandler.h"  // <-- Handles file uploads
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/URI.h>
#include <iostream>
#include <sstream>

using namespace Poco::Net;
using namespace std;

RequestHandler::RequestHandler(UserManager& userManager)
    : _userManager(userManager) {}

void RequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    std::cout << "[DEBUG] Received request: " << request.getMethod() << " " << request.getURI() << std::endl;

    const std::string& uri = request.getURI();

    if (uri.find("/register") == 0) {
        std::cout << "[DEBUG] Routing to /register" << std::endl;
        handleRegister(request, response);
    } else if (uri.find("/login") == 0) {
        std::cout << "[DEBUG] Routing to /login" << std::endl;
        handleLogin(request, response);
    } else if (uri.find("/upload") == 0 && request.getMethod() == HTTPRequest::HTTP_POST) {
        std::cout << "[DEBUG] Routing to /upload" << std::endl;
        try {
            UploadHandler uploadHandler;
            uploadHandler.handleRequest(request, response);
        } catch (const std::exception& ex) {
            std::cerr << "[ERROR] UploadHandler threw exception: " << ex.what() << std::endl;
            response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            ostream& out = response.send();
            out << "Internal Server Error during file upload." << std::endl;
        } catch (...) {
            std::cerr << "[ERROR] Unknown error in UploadHandler" << std::endl;
            response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            response.setContentType("text/plain");
            ostream& out = response.send();
            out << "Unknown error occurred during file upload." << std::endl;
        }
    } else {
        std::cout << "[DEBUG] 404 Not Found for URI: " << uri << std::endl;
        response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        response.setContentType("text/plain");
        ostream& out = response.send();
        out << "404 Not Found" << std::endl;
    }
}

void RequestHandler::handleRegister(HTTPServerRequest& request, HTTPServerResponse& response) {
    std::string username, password;
    extractCredentials(request, username, password);

    std::cout << "[DEBUG] Registering user: " << username << std::endl;

    bool success = _userManager.registerUser(username, password);

    response.setStatus(success ? HTTPResponse::HTTP_OK : HTTPResponse::HTTP_CONFLICT);
    response.setContentType("text/plain");
    ostream& out = response.send();
    if (success) {
        out << "User registered successfully" << std::endl;
    } else {
        out << "User already exists" << std::endl;
    }
}

void RequestHandler::handleLogin(HTTPServerRequest& request, HTTPServerResponse& response) {
    std::string username, password;
    extractCredentials(request, username, password);

    std::cout << "[DEBUG] Logging in user: " << username << std::endl;

    bool success = _userManager.loginUser(username, password);

    response.setStatus(success ? HTTPResponse::HTTP_OK : HTTPResponse::HTTP_UNAUTHORIZED);
    response.setContentType("text/plain");
    ostream& out = response.send();
    if (success) {
        out << "Login successful" << std::endl;
    } else {
        out << "Invalid credentials" << std::endl;
    }
}

std::string RequestHandler::readRequestBody(HTTPServerRequest& request) {
    istream& in = request.stream();
    ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

void RequestHandler::extractCredentials(HTTPServerRequest& request, std::string& username, std::string& password) {
    if (request.getMethod() == "GET") {
        Poco::URI uri(request.getURI());
        auto params = uri.getQueryParameters();
        for (const auto& param : params) {
            if (param.first == "username") username = param.second;
            if (param.first == "password") password = param.second;
        }
    } else if (request.getMethod() == "POST") {
        std::string body = readRequestBody(request);
        std::istringstream iss(body);
        iss >> username >> password;
    }
}
