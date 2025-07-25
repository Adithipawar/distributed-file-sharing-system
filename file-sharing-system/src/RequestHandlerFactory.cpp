#include "RequestHandlerFactory.h"
#include "UploadHandler.h"
#include "DownloadHandler.h"
#include "ShareHandler.h"
#include "SharedWithHandler.h"  // ✅ Include new handler
#include "RequestHandler.h"

RequestHandlerFactory::RequestHandlerFactory(UserManager& userManager)
    : _userManager(userManager) {}

Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request) {
    const std::string& uri = request.getURI();

    if (uri.find("/upload") == 0) {
        return new UploadHandler();
    } else if (uri.find("/download") == 0) {
        return new DownloadHandler();
    } else if (uri.find("/share") == 0) {
        return new ShareHandler();
    } else if (uri.find("/shared-with") == 0) {  // ✅ New route
        return new SharedWithHandler();
    }

    return new RequestHandler(_userManager);
}
