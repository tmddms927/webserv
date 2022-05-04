#include "Server.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"

int main() {
    // config ();
    run:
        try {
            Server server;
            server.socketRun();
        }
        catch (std::exception e){
            std::cout << e.what() << std::endl;
            std::cout << "webserv restart..." << std::endl;
            goto run;
        }
}
