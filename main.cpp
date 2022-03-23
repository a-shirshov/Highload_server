#include <iostream>
#include "lib/server/server.h"
#include <fstream>
#include <sstream>

const std::string configPath = "../httpd.conf";


Config parseConfig(std::string configPath) {
    Config conf;

    std::ifstream confFile(configPath);
    std::string threads_limit, document_root;

    confFile >> threads_limit >> conf.threads_limit >> document_root >> conf.document_root;

    confFile.close();

    return conf;
}


int main() {
    Config conf;
    conf = parseConfig(configPath);
    conf.port = 80;
    Server httpServer(conf);
    httpServer.Start();
    return 0;
}