#include "UploadHandler.h"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/PartHandler.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/MessageHeader.h>
#include <Poco/FileStream.h>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Exception.h>
#include <libpq-fe.h>
#include <iostream>

using namespace Poco;
using namespace Poco::Net;
using namespace std;

// Handle file part
class MyPartHandler : public PartHandler {
public:
    std::string savedFilePath;
    std::string savedFileName;
    long savedFileSize = 0;

    void handlePart(const MessageHeader& header, std::istream& stream) override {
        if (header.has("Content-Disposition")) {
            std::string disposition = header["Content-Disposition"];
            std::string filename;

            std::size_t pos = disposition.find("filename=");
            if (pos != std::string::npos) {
                filename = disposition.substr(pos + 9);
                if (!filename.empty() && filename.front() == '"') filename.erase(0, 1);
                if (!filename.empty() && filename.back() == '"') filename.pop_back();

                std::string uploadDir = "uploads";
                File dir(uploadDir);
                if (!dir.exists()) {
                    dir.createDirectories();
                }

                std::string filePath = Path(uploadDir, filename).toString();
                FileOutputStream fileStream(filePath);
                fileStream << stream.rdbuf();  // Write entire stream to file
                fileStream.close();

                savedFileName = filename;
                savedFilePath = filePath;
                File uploaded(filePath);
                savedFileSize = uploaded.getSize();

                std::cout << "✅ File saved to: " << filePath << std::endl;
            }
        }
    }
};

UploadHandler::UploadHandler() {}

void UploadHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    response.setContentType("text/plain");
    std::ostream& out = response.send();

    try {
        MyPartHandler partHandler;
        HTMLForm form(request, request.stream(), partHandler);

        if (!partHandler.savedFileName.empty()) {
            // Connect to Yugabyte using libpq
            PGconn* conn = PQconnectdb("host=127.0.0.1 port=5433 user=yugabyte password=yugabyte dbname=yugabyte");

            if (PQstatus(conn) != CONNECTION_OK) {
                out << "❌ Database connection failed: " << PQerrorMessage(conn) << "\n";
                PQfinish(conn);
                return;
            }

            std::cout << "✅ Connected to YugabyteDB\n";

            const char* query = "INSERT INTO files (id, filename, filepath, uploader, upload_time, filesize) "
                                "VALUES (gen_random_uuid(), $1, $2, $3, now(), $4)";

            const char* paramValues[4];
            std::string uploader = "default_user";
            std::string sizeStr = std::to_string(partHandler.savedFileSize);

            paramValues[0] = partHandler.savedFileName.c_str();
            paramValues[1] = partHandler.savedFilePath.c_str();
            paramValues[2] = uploader.c_str();
            paramValues[3] = sizeStr.c_str();

            PGresult* res = PQexecParams(
                conn, query, 4, nullptr, paramValues, nullptr, nullptr, 0
            );

            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                std::cerr << "❌ Insert failed: " << PQerrorMessage(conn) << std::endl;
                out << "Insert failed: " << PQerrorMessage(conn) << "\n";
            } else {
                std::cout << "✅ Metadata inserted into DB\n";
                out << "File uploaded and metadata saved successfully.\n";
            }

            PQclear(res);
            PQfinish(conn);
        } else {
            out << "⚠️ No file uploaded.\n";
        }

    } catch (const Exception& ex) {
        response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "Upload failed: " << ex.displayText() << "\n";
    }
}
