#include "ShareHandler.h"
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>
#include <libpq-fe.h>
#include <iostream>
#include <sstream>

void ShareHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                 Poco::Net::HTTPServerResponse& response) {
    std::ostream& out = response.send();
    Poco::URI uri(request.getURI());

    std::string filename, sharedWith;
    for (const auto& param : uri.getQueryParameters()) {
        if (param.first == "filename") filename = param.second;
        else if (param.first == "shared_with") sharedWith = param.second;
    }

    if (filename.empty() || sharedWith.empty()) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        out << "Missing filename or shared_with parameter.\n";
        return;
    }

    std::cout << "[DEBUG] Sharing file: " << filename << " with " << sharedWith << std::endl;

    PGconn* conn = PQconnectdb("host=127.0.0.1 port=5433 user=yugabyte password=yugabyte dbname=yugabyte");
    if (PQstatus(conn) != CONNECTION_OK) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "DB connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    const char* param[1] = { filename.c_str() };
    PGresult* res = PQexecParams(conn,
        "SELECT id FROM files WHERE filename = $1 LIMIT 1",
        1, nullptr, param, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
        out << "File not found.\n";
        std::cout << "[ERROR] File not found in DB for sharing.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }

    std::string fileId = PQgetvalue(res, 0, 0);
    PQclear(res);

    std::cout << "[DEBUG] File ID found: " << fileId << std::endl;

    // Optional TODO: replace "default_user" with actual session user later
    std::string sharedBy = "default_user";

    std::stringstream query;
    query << "INSERT INTO shared_files (file_id, shared_with, shared_by) VALUES ('"
          << fileId << "', '"
          << sharedWith << "', '"
          << sharedBy << "');";

    std::cout << "[DEBUG] Executing query: " << query.str() << std::endl;

    PGresult* insertRes = PQexec(conn, query.str().c_str());

    if (PQresultStatus(insertRes) != PGRES_COMMAND_OK) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "Failed to insert sharing info.\n";
        std::cerr << "[ERROR] Insert failed: " << PQerrorMessage(conn) << std::endl;
    } else {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
        out << "File shared successfully with " << sharedWith << ".\n";
    }

    PQclear(insertRes);
    PQfinish(conn);
}
