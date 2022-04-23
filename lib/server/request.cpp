#include <request.h>

void parseFirstLineRequest(Request& result, std::string request) {
    std::string token;
    int param_index = 0;
    
    for (int i = 0; i < request.size(); i++) {
        if (request[i] == ' ' || request[i] == '\r') {
            switch (param_index) {
                case 0:
                    result.method = token;
                    break;
                case 1:
                    result.path = token;
                    break;
                case 2:
                    result.protocol = token;
                    break;
            }

            if (param_index >= 2) {
                break;
            }

            param_index++;
            token.clear();
        } else {
            token += request[i];
        }
        
    }
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