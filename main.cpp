#include "Server.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"

void seungoh_config_test(std::vector<servers> & server) {
    servers temp;
    temp.host = "127.0.0.1";
    temp.location = "/";
    temp.port = 80;
    server.push_back(temp);
//////////////////////////////////
    temp.host = "127.0.0.1";
    temp.location = "/";
    temp.port = 8080;
    server.push_back(temp);
}

int main() {
    Config c;
    std::vector<servers> ss;
    seungoh_config_test(ss);

    run:
        try {
            Server server(ss, c);
            server.socketRun();
        }
        catch (std::exception e){
            std::cout << e.what() << std::endl;
            std::cout << "webserv restart..." << std::endl;
            goto run;
        }
}
