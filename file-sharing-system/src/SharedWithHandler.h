#ifndef SHAREDWITHHANDLER_H
#define SHAREDWITHHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

class SharedWithHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

#endif // SHAREDWITHHANDLER_H
