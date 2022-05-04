#include "SocketController.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"
#include "Config.hpp"
int main() {
    Config conf;
    conf.runParse();
    run:
        try {
            SocketController server;
            server.socketRun();
        }
        catch (std::exception e){
            goto run;
        }
}
