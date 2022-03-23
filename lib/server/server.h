#include <iostream>

struct Config {
    int threads_limit;
    std::string document_root;
    uint16_t port;
};

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