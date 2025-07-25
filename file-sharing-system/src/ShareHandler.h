#ifndef SHARE_HANDLER_H
#define SHARE_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

class ShareHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

#endif
