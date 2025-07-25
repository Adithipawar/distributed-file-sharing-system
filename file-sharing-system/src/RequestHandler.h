#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <string>
#include "UserManager.h"

class RequestHandler : public Poco::Net::HTTPRequestHandler {
public:
    RequestHandler(UserManager& userManager);

    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;

private:
    UserManager& _userManager;

    void handleRegister(Poco::Net::HTTPServerRequest& request,
                        Poco::Net::HTTPServerResponse& response);

    void handleLogin(Poco::Net::HTTPServerRequest& request,
                     Poco::Net::HTTPServerResponse& response);

    std::string readRequestBody(Poco::Net::HTTPServerRequest& request);
    
    // NEW: for handling GET or POST credential extraction
    void extractCredentials(Poco::Net::HTTPServerRequest& request,
                            std::string& username, std::string& password);
};

#endif // REQUEST_HANDLER_H
