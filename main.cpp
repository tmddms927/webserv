#include "Server.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"

void seungoh_config_test(Config & c) {
    servers_s temp;
    temp.host = "127.0.0.1";
    temp.location = "/";
    temp.port = 80;
    c.config.
    config.config.push_back(temp);
//////////////////////////////////
    temp.host = "127.0.0.1";
    temp.location = "/";
    temp.port = 8080;
    servers.push_back(temp);
}

int main() {
    Config c;
    seungoh_config_test(c);

    run:
        try {
            Server server(servers);
            server.socketRun();
        }
        catch (std::exception e){
            std::cout << e.what() << std::endl;
            std::cout << "webserv restart..." << std::endl;
            goto run;
        }
}
