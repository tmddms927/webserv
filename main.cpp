#include "Server.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"

int main() {
    std::vector<servers> ss;

    Config conf;
    try {
        conf.runParse();
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    run:
        try {
            Server server(conf);
            server.socketRun();
        }
        catch (std::exception e){
            // std::cout << e.what() << std::endl;
            std::cout << "webserv restart..." << std::endl;
            goto run;
        }
}
