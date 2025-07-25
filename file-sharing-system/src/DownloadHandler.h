#ifndef DOWNLOAD_HANDLER_H
#define DOWNLOAD_HANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

class DownloadHandler : public Poco::Net::HTTPRequestHandler {
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

#endif
