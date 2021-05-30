//
// Created by Harren Leilani on 5/23/21.
//

#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP


#include "Server.hpp"
#include "../Helpers/Reader.hpp"
#include "../HTTP/Request.hpp"
#include "ctime"
#include "ServerListener.hpp"

enum state
        {
    requestParsing,
    sendingResponse,
    closeConnection
        };

class ServerListener;

using namespace std;

#ifndef READ_SIZE
#define READ_SIZE 32768
#endif

class Client {
private:
    Server attachedServer;
    sockaddr_in addr;
    int sock;
    string readBuffer;
    string sendBuffer;
    Reader *reader;
	Config *config;
	long read_count;
	bool    waitingRead;

	string responseBuffer = "";

	Client();
public:
    state currentState;
    Server *server;
    Http::Request request;
    Http::Response response;


    Client &operator=(const Client &src);
    Client(const Client&);
    ~Client();
    int onError(int code);
    int readRequest(ServerListener &listener);
    Client(int sock, sockaddr_in addr);
    time_t lastOperationTime;
    int getSock();
    int sendResponse();
    int readRequest();

	int checkStatus(int status);
};


#endif //WEBSERV_CLIENT_HPP