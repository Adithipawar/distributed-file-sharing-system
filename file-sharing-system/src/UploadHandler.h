#ifndef UPLOADHANDLER_H
#define UPLOADHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

// UploadHandler handles POST requests to /upload endpoint
class UploadHandler : public Poco::Net::HTTPRequestHandler {
public:
    UploadHandler();
    void handleRequest(Poco::Net::HTTPServerRequest& request,
                       Poco::Net::HTTPServerResponse& response) override;
};

#endif // UPLOADHANDLER_H
