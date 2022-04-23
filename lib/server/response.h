#include <vector>
#include <iostream>
#include <map>

struct Response {
    int code;
    std::map<std::string, std::string> Headers;
    std::string file;
    int bodyLength = 0;
    std::string type = "";

    std::string getStringResponse();
    void setHeaders();
    std::string buildResponse();
};

