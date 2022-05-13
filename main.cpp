#include "Server/Server.hpp"
#include "HTTP/HeaderFieldInfo.hpp"
#include "HTTP/HTTP.hpp"

int main(int argc, char *argv[]) {
    std::vector<servers> ss;

    if (argc != 2)
        exit(1);
    Config conf(argv[1]);
    try {
        conf.runParse();
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    // run:
        // try {
            Server server(conf);
            server.socketRun();
        // }
        // catch (std::exception e){
        //     // std::cout << e.what() << std::endl;
        //     std::cout << "webserv restart..." << std::endl;
        //     goto run;
        // }
}
