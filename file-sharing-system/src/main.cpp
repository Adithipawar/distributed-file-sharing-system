#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>

#include "RequestHandlerFactory.h"
#include "UserManager.h"

using namespace Poco::Net;
using namespace Poco::Util;
using namespace std;

class FileSharingServer : public ServerApplication {
protected:
    int main(const vector<string>&) override {
        UserManager userManager;

        // Create server and factory
        HTTPServer server(
            new RequestHandlerFactory(userManager),
            ServerSocket(8080),
            new HTTPServerParams
        );

        server.start();
        cout << "Server started on port 8080" << endl;

        waitForTerminationRequest(); // Wait for CTRL+C
        server.stop();
        cout << "Server stopped" << endl;
        return Application::EXIT_OK;
    }
};

int main(int argc, char** argv) {
    FileSharingServer app;
    return app.run(argc, argv);
}
