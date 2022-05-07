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
    }


    const std::vector<servers> & aa = conf.getConfig();
    // for (int i = 0; i < aa.size(); ++i)
	// {
    //     std::cout << aa[i].cgi << ", ";
    //     std::cout << aa[i].host << ", ";
    //     std::cout << aa[i].location << ", ";
    //     std::cout << aa[i].port << ", ";
    //     std::cout << aa[i].root << std::endl;
    // }


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
