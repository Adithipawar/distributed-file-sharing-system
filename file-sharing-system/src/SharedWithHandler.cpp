#include "SharedWithHandler.h"
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>
#include <libpq-fe.h>
#include <iostream>
#include <sstream>

void SharedWithHandler::handleRequest(Poco::Net::HTTPServerRequest& request,
                                      Poco::Net::HTTPServerResponse& response) {
    std::ostream& out = response.send();
    Poco::URI uri(request.getURI());

    std::string username;
    for (const auto& param : uri.getQueryParameters()) {
        if (param.first == "username") username = param.second;
    }

    if (username.empty()) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
        out << "Missing username parameter.\n";
        return;
    }

    PGconn* conn = PQconnectdb("host=127.0.0.1 port=5433 user=yugabyte password=yugabyte dbname=yugabyte");
    if (PQstatus(conn) != CONNECTION_OK) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "DB connection failed: " << PQerrorMessage(conn);
        PQfinish(conn);
        return;
    }

    const char* param[1] = { username.c_str() };
    PGresult* res = PQexecParams(conn,
        "SELECT f.filename FROM shared_files s JOIN files f ON s.file_id = f.id WHERE s.shared_with = $1",
        1, nullptr, param, nullptr, nullptr, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        response.setStatus(Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR);
        out << "Query failed.\n";
        PQclear(res);
        PQfinish(conn);
        return;
    }

    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
    response.setContentType("application/json");

    std::stringstream json;
    json << "[";

    for (int i = 0; i < PQntuples(res); ++i) {
        if (i > 0) json << ",";
        json << "\"" << PQgetvalue(res, i, 0) << "\"";
    }

    json << "]";

    out << json.str();

    PQclear(res);
    PQfinish(conn);
}
