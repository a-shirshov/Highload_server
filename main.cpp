#include <iostream>
#include "lib/server/server.h"
#include "lib/config/config.h"

const std::string configPath = "../httpd.conf";

int main() {
    Config conf;
    conf = parseConfig(configPath);
    conf.port = 80;
    Server httpServer(conf);
    httpServer.Start();
    return 0;
}