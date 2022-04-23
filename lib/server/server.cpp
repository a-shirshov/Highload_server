#include "server.h"
#include <netinet/in.h>
#include <unistd.h>
#include "response.h"
#include "request.h"
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <signal.h>
#include <chrono>
#include <thread>


Server::Server(Config conf) {
    _port = conf.port;
    _threads_limit = conf.threads_limit;
    _document_root = conf.document_root;
}

Server::~Server() {
    close(server_socket);
}

void Server::Start() {
    signal(SIGPIPE,SIG_IGN);
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (this->server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 256) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    Serve(&server_socket);
}

void Server::Serve(int *sockfd) {
    while (true) {
        struct sockaddr_in clientAddr;
        int clen = sizeof(clientAddr);
        int *isock = (int *)malloc(sizeof(int));
        if ((*isock = accept(*sockfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clen)) < 0) {
            perror("accept");
            Serve(sockfd);
        }
        std::thread threadPerRequest(HandleV2, isock);
        threadPerRequest.join();
    }
}

std::string getType(std::string &path) {
    std::string type = "";
    int lastdot = path.find_last_of('.');
    if (lastdot != std::string::npos) {
        type = path.substr(lastdot + 1,path.length() - 1);
    }

    std::map <std::string, int> mapping;
    mapping["txt"] = 0;
    mapping["css"] = 1;
    mapping["html"] = 2;
    mapping["js"] = 3;
    mapping["jpg"] = 4;
    mapping["png"] = 5;
    mapping["jpeg"] = 6;
    mapping["gif"] = 7;
    mapping["swf"] = 8;



    switch (mapping[type]) {
    case 0:
        type = "text/txt";
        break;
    case 1:
        type = "text/css";
        break;
    case 2:
        type = "text/html";
        break;
    case 3:
        type = "application/javascript";
        break;
    case 4:
        type = "image/jpeg";
        break;
    case 5:
        type = "image/png";
        break;
    case 6:
        type = "image/jpeg";
        break;
    case 7:
        type = "image/gif";
        break;
    case 8:
        type = "application/x-shockwave-flash";
        break;
    default:
        type = "";
    }

    return type;
}

int readFile(std::string path, std::string &respFile) {
    std::streampos size;
    char * memblock;
    std::ifstream file(path,std::ios::binary | std::ios::in);
    if (file.is_open()) {
        //Very long operation
        //std::string fileData = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        //respFile = fileData;
        
        //Fast but not working
        /*
        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, std::ios::beg);
        file.read (memblock, size);
        respFile = std::string(memblock);
        delete[]memblock;
        */

        //It works!!!
        std::stringstream fileData;
        fileData << file.rdbuf();
        respFile = fileData.str();

        file.close();
        return 0;
    }
    return 1;
}

int getFileByPath(std::string path, std::string& fileData) {
    if (path.back() == '/') {
        if (path.find(".html/") != std::string::npos) {
            return 404;
        }
        path += "index.html";
        int code = readFile(path, fileData);
        if (code != 0) {
            fileData.clear();
            return 403;
        }
    }
    //auto t1 = std::chrono::high_resolution_clock::now();
    int code = readFile(path,fileData);
    //auto t2 = std::chrono::high_resolution_clock::now();
    //auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    //printf("%s%ld%s\n","Time is ",ms_int.count(),"ms");
    if (code != 0) {
            fileData.clear();
            return 404;
    }
    return 200;
}

void Server::HandleV2(int* socket) {
    int isock = *socket;
    std::string buffer;
    buffer.resize(10000);
    int readBytes = recv(isock, (char *) buffer.c_str(), 10000, 0);
    if (readBytes == -1) {
        perror("recv");
        close(isock);
    }

    Request request;
    Response response;
    
    parseFirstLineRequest(request, buffer);
    buffer.clear();

    
    request.path = ".." + request.path;
    request.path = urlDecode(request.path);

    size_t pos = request.path.find('?',0);
    if (pos != std::string::npos) {
        request.path = request.path.substr(0,pos);
    }
    if (request.method == "HEAD" || request.method == "GET") {
        if(request.path.find("../",3) != std::string::npos) {
            response.code = 403;
        } else {
            
            response.code = getFileByPath(request.path, response.file);
            if (response.code == 200) {
                response.bodyLength = response.file.length();
            }
            response.type = getType(request.path);
            if (request.method == "HEAD") {
                response.file.clear();
            }
        }
    } else {
        response.code = 405;
    }
    
    std::string responseString = response.buildResponse();
    
    send(isock, responseString.c_str(),responseString.size(), 0);
    close(isock);
}
