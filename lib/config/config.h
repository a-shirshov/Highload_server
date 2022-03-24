#ifndef CONFIG_H
#define CONFIG_H
#include <iostream>
#include <fstream>

struct Config {
    int threads_limit;
    std::string document_root;
    uint16_t port;
};

Config parseConfig(std::string configPath);

#endif