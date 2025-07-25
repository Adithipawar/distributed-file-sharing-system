#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include "UserManager.h"
#include "RequestHandler.h"

class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
public:
    RequestHandlerFactory(UserManager& userManager);
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;

private:
    UserManager& _userManager;
};
