#include <iostream>

struct Request {

    std::string method;
    std::string path;
    std::string protocol;
    
};

void parseFirstLineRequest(Request& result, std::string request);
std::string urlDecode(std::string &SRC);