#include <response.h>
#include <ctime>
std::string Response::getStringResponse() {
    std::string response;
    response += "HTTP/1.1 " + this->status + "\r\n";
    for(std::map<std::string,std::string>::iterator it = this->Headers.begin(); it != this->Headers.end(); it++) {
        std::string header = it->first;
        std::string value = it->second;
        response += header + ": " + value + "\r\n";
    }

    response += "\r\n";

    //Body
    if (!this->file.empty()) {
        response += this->file;
    }

    return response;
}

void Response::setHeaders() {
    this->Headers.insert({"Connection","close"});
    this->Headers.insert({"Server","MyServer/1.0.0 (Unix)"});
    std::time_t now = time(0);
    tm *gmtm = gmtime(&now);
    char *date = asctime(gmtm);
    std::string strdate = std::string(date);
    //Delete /n
    strdate.pop_back();
    this->Headers.insert({"Date",strdate});

    if (this->bodyLength != 0) {
        this->Headers.insert({"Content-Length",std::to_string(bodyLength)});
    }

    if (this->type != "") {
         this->Headers.insert({"Content-Type",this->type});
    }
}