#include "server.h"
#include <netinet/in.h>
#include <unistd.h>
#include "response.h"
#include <ctime>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>


Server::Server(Config conf) {
    _port = conf.port;
    _threads_limit = conf.threads_limit;
    _document_root = conf.document_root;
}

Server::~Server() {
    
}

void Server::Start() {
    int sockfd;
    int opt = 1;
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
/*
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
*/

    printf("%d\n",sockfd);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 256) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    Serve(&sockfd);
}

void Server::Serve(int *sockfd) {
    while (true) {
        struct sockaddr_in clientAddr;
        int clen = sizeof(clientAddr);
        int *isock = (int *)malloc(sizeof(int));
        if ((*isock = accept(*sockfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clen)) < 0) {
            perror("accept");
            continue;
        }
        pthread_t request;
        pthread_create(&request, nullptr, Handle, (void *) isock);
        //sleep(5);
    }
}



bool isMethodCorrect(std::string request, std::string &method) {
    std::string firstLine;
    int posEOL = request.find("\r\n");
    firstLine = request.substr(0,posEOL);

    int posSpace = firstLine.find(" ");
    method = firstLine.substr(0,posSpace);

    if (method == "GET" || method == "HEAD") {
        return true;
    }

    return false;
}

//https://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string urlDecode(std::string &SRC) {
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<SRC.length(); i++) {
        if (int(SRC[i])==37) {
            sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else {
            ret+=SRC[i];
        }
    }
    return (ret);
}

std::string getPath(std::string request) {
    std::string firstLine;
    int posEOL = request.find("\r\n");
    firstLine = request.substr(0,posEOL);

    printf("%s%s\n","Firstline: ",(char *)firstLine.c_str());

    std::string path = "../var/www/html";
    int pathStart = firstLine.find(" ") + 1;
    int pathEnd = firstLine.find(" ",pathStart);
    if (pathEnd > firstLine.find("?",pathStart)) {
        pathEnd = firstLine.find("?",pathStart);
    }

    for (int i = pathStart; i < pathEnd; i++) {
        path += firstLine[i];
    }

    printf("%s%s\n","Path: ",(char *)path.c_str());

    path = urlDecode(path);

    printf("%s%s\n","Path Decode: ",(char *)path.c_str());

    if (path.find("../",3) != path.npos) {
        path.clear();
        return path;
    }

    if (path[path.size()-1] == '/') {
        printf("%s%s\n","Directory check in fun: ",(char *)path.c_str());
        if(std::filesystem::exists(path.c_str())) {
            auto it = std::filesystem::directory_iterator(path);
            std::vector<std::filesystem::path> files;

            std::copy_if(std::filesystem::begin(it), std::filesystem::end(it), std::back_inserter(files), 
            [](const auto& entry) {
                return std::filesystem::is_regular_file(entry);
            });

            printf("%s%ld\n","Files Length: ",files.size());
            std::string indexFile;
            indexFile.clear();
            if (files.size() == 0) {
                return indexFile;
            }

            for (auto& fileIt : files) {
                printf("%s%s\n","Filename: ",(char *)fileIt.filename().string().c_str());
                std::string filename = fileIt.filename().string();
                int lastNameSymbolIndex =  filename.find_last_of(".");
                std::string rawname = filename.substr(0,lastNameSymbolIndex);
                if (strcmp(rawname.c_str(),"index") == 0) {
                    printf("%s%s\n","Index: ",(char *)rawname.c_str());
                    indexFile = fileIt.string();
                    return indexFile;
                }
            }

            return indexFile;
        } 
    }

    return path;
}

std::string getType(std::string &path) {
    std::string type;
    for (int i = path.size() - 1; i >= 0; i--) {
        if (path[i] == '.') {
            break;
        }
        type += path[i];
    }

    std::reverse(type.begin(),type.end());

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

std::string readFile(std::string path, std::string &respFile) {
    std::fstream file(path,std::ios::binary | std::ios::in);
    if (file.is_open()) {
        std::string fileData = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        respFile = fileData;
        file.close();
    }
    return respFile;
}

Response getResponse(std::string request) {
    printf("%s\n","In Response");
    Response response;
    std::string path;
    if (path[path.size()-1] == '/') {
        if(std::filesystem::exists(path.c_str())) {
            response.status = "404 Not Found";
            response.file.clear();
            response.bodyLength = 0;
            return response;
        }
    }

    path = getPath(request);
    if (path.empty()) {
        response.status = "403 Forbidden";
        response.bodyLength = 0;
        return response;
    }

    printf("%s\n","Path check");

    response.file = readFile(path,response.file);

    printf("%s\n","Read check");
    if (response.file.empty()) {
        response.status = "404 Not Found";
        response.bodyLength = 0;
    } else {
        response.status = "200 OK";
        response.bodyLength = response.file.size();
    }

    response.type = getType(path);
    if (response.type.empty()) {
        response.status = "403 Forbidden";
        response.file.clear();
        response.bodyLength = 0;
    }

    return response;
}

void *Server::Handle(void *arg) {
    int isock = *(int *) arg;
    std::string buffer;
    buffer.resize(10000);
    int readBytes = recv(isock, (char *) buffer.c_str(), 10000, 0);
    if (readBytes == -1) {
        perror("recv");
        free(arg);
        close(isock);
        pthread_exit(0);
    }
    printf("%s\n",(char *)buffer.c_str());
    Response response;
    std::string method;

    if (isMethodCorrect(buffer,method)) {
        response = getResponse(buffer);
    } else {
        response.status = "405 Method Not Allowed";
    }

    if(strcmp(method.c_str(),"HEAD") == 0) {
        response.file.clear();
    }

    response.setHeaders();
    std::string responseString = response.getStringResponse();
    printf("%s\n",(char *)responseString.c_str());
    send(isock, responseString.c_str(),responseString.size(), 0);
    close(isock);
    free(arg);
    pthread_exit(0);
}

