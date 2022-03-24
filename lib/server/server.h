#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include "../config/config.h"


class Server {
public:
    Server(Config conf);
    ~Server();
    void Start();

private:
    uint16_t _port;
    int _threads_limit;
    std::string _document_root;

    static void Serve(int *sockfd);
    static void *Handle(void *arg);
};

#endif