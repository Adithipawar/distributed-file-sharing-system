#include "DownloadHandler.h"
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/MessageHeader.h>
#include <Poco/FileStream.h>
#include <Poco/File.h>
#include <libpq-fe.h>
#include <iostream>
#include <Poco/StreamCopier.h>

using namespace Poco;
using namespace Poco::Net;

void DownloadHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response) {
    std::ostream& out = response.send();

    try {
        std::string filename;
        if (request.getURI().find("?filename=") != std::string::npos) {
            filename = request.getURI().substr(request.getURI().find("?filename=") + 10);
        }

        if (filename.empty()) {
            response.setStatus(HTTPResponse::HTTP_BAD_REQUEST);
            out << "Missing filename parameter.\n";
            return;
        }

        // Connect to DB
        PGconn* conn = PQconnectdb("host=127.0.0.1 port=5433 user=yugabyte password=yugabyte dbname=yugabyte");
        if (PQstatus(conn) != CONNECTION_OK) {
            response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
            out << "Database connection failed: " << PQerrorMessage(conn) << "\n";
            PQfinish(conn);
            return;
        }

        // Query the file path
        const char* param[1] = { filename.c_str() };
        PGresult* res = PQexecParams(
            conn,
            "SELECT filepath FROM files WHERE filename = $1 LIMIT 1",
            1, nullptr, param, nullptr, nullptr, 0
        );

        if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
            response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            out << "File not found.\n";
            PQclear(res);
            PQfinish(conn);
            return;
        }

        std::string filePath = PQgetvalue(res, 0, 0);
        PQclear(res);
        PQfinish(conn);

        File file(filePath);
        if (!file.exists()) {
            response.setStatus(HTTPResponse::HTTP_NOT_FOUND);
            out << "File not found on disk.\n";
            return;
        }

        response.setContentType("application/octet-stream");
        response.setContentLength(file.getSize());
        response.set("Content-Disposition", "attachment; filename=\"" + filename + "\"");

        FileInputStream fis(filePath);
        StreamCopier::copyStream(fis, out);
    } catch (const std::exception& ex) {
        response.setStatus(HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "Download failed: " << ex.what() << "\n";
    }
}
