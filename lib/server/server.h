#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include "../config/config.h"


class Server {
public:
    explicit Server(Config conf);
    ~Server();
    void Start();

private:
    uint16_t _port;
    int _threads_limit;
    std::string _document_root;
    int server_socket;

    static void Serve(int *sockfd);
    //Deprecated
    static void *Handle(void *arg);
    static void HandleV2(int* isock);
};

#endif