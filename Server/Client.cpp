//
// Created by Harren Leilani on 5/23/21.
//

#include "Client.hpp"

Client::Client(int sock, sockaddr_in addr)
{
    currentState = requestParsing;
    reader = new Reader();
    this->sock = sock;
    this->addr = addr;
    time(&lastOperationTime);
}

int Client::getSock() {
    return sock;
}


int Client::sendResponse() {
    time(&lastOperationTime);
}

int Client::onError(int code)
{
    request.getLog(code);
    response.code(code);
    if (code != 505)
        response.attachDefaultHeaders(this->server->serverConfig);
    currentState = sendingResponse;
    return 1;
}
int Client::readRequest(ServerListener &listener, int socket)
{
    string result;
    Config *config;
    int status;
    int read_count;
    int BodyLimit;
    // Querry
    if (!request.query.is_set) {
        status = reader->readLine(result, socket, READ_SIZE * 2);
        if (status)
            return status;
        request.parseQuery(result);
        if (request.query.protocol != "1.1")
            return onError(505);
    }
    // Headers
    if (request.headers.empty()) {
        status = reader->readBefore(result, socket, "\r\n\r\n", READ_SIZE);
        if (status)
            return status;
        // TODO: max size 413
        request.parseHeaders(result);
        if (!this->server) {
            string host = request.headers["Host"];
            server = &listener.FindServerByHost(host);
            server = &server->GetLocationServer(request.query.address);
        }
        config = &server->serverConfig;
        BodyLimit = config->limitClientBodySize;
        if (config->auth) {
            if (!request.headers.count("Authorization"))
                return onError(401);
            string authEncoded = request.headers["Authorization"];
            authEncoded = string(find(authEncoded.begin(), authEncoded.end(), ' ') + 1, authEncoded.end());
            if (find(config->auth_file_content.begin(), config->auth_file_content.end(), authEncoded) ==
                config->auth_file_content.end())
                return onError(401);
            //TODO: Проверить правильны ли пришедшие данные, с файла с авторизациями
        }
        if (!config->allowedFunctions.empty() && config->allowedFunctions.find(request.query.method) == -1)
            return onError(405);
//    if (!config->allowedFunctions.empty() && config->allowedFunctions.find(request.query.method) == -1)
//       throw Http::http_exception(405, request.getLog(405), config);
        if (request.headers.count("Content-Length"))
            if (stoi(request.headers["Content-Length"]) > BodyLimit)
                return onError(405);
//    if (request.headers.count("Content-Length"))
//       if (stoi(request.headers["Content-Length"]) > BodyLimit)
//          throw Http::http_exception(413, request.getLog(413), config);
    }
    // Body
    if (request.headers["Transfer-Encoding"] == "chunked") {
        status = reader->readLine(result, socket, READ_SIZE);
        if (status)
            return status;
        read_count = stoi(result, nullptr, 16);
        status = reader->readCount(result, read_count, socket);
        if (status)
            return status;
        request.body += result;
        if (request.body.length() > BodyLimit)
            return onError(413);
    } else {
        if (request.headers.count("Content-Length")) {
            status = reader->readCount(result, stoi(request.headers["Content-Length"]), socket);
            if (status)
                return status;
        } else {
            status = reader->readCount(result, READ_SIZE, socket);
            if (status)
                return status;
        }
        request.body = result;
    }
    try {
        response = server->SendHttpResponse(addr, socket, &request, config);
        if (response.code() >= 200 && response.code() < 300) {
            currentState = sendingResponse;
            return 1;
        }
        onError(response.code());
    } catch (exception &e) {
        response.code(500);
        try {
            response.putFile(DEFAULT_ERROR_PAGE);
        } catch (exception &e) {
            cerr << e.what() << endl;
        }
        onError(500);
    }
}

Client &Client::operator=(const Client &src) {
    this->lastOperationTime = src.lastOperationTime;
    this->sock = src.sock;
    this->request = src.request;
    this->response = src.response;
    this->currentState = src.currentState;
    this->readBuffer = src.readBuffer;
    this->sendBuffer = src.sendBuffer;
    this->attachedServer = src.attachedServer;
    this->addr = src.addr;
    return *this;
}

Client::Client(const Client &c) {
    *this = c;
}

Client::~Client() {
    delete reader;
}
