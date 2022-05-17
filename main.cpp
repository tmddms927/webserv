#include "Server/Server.hpp"
#include "HTTP/HeaderFieldInfo.hpp"
#include "HTTP/HTTP.hpp"

int main(int argc, char *argv[]) {
    std::vector<servers> ss;

    if (argc != 2) {
        std::cout << "you need config file!" << std::endl;
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);
    Config conf(argv[1]);
    try {
        conf.runParse();
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    Server server(conf);
    server.socketRun();
}
