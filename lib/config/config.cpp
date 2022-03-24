#include "config.h"

Config parseConfig(std::string configPath) {
    Config conf;

    std::ifstream confFile(configPath);
    std::string threads_limit, document_root;

    confFile >> threads_limit >> conf.threads_limit >> document_root >> conf.document_root;

    confFile.close();

    return conf;
}