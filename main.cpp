#include "SocketController.hpp"
#include "HeaderFieldInfo.hpp"
#include "HTTP.hpp"

int main() {
    // config ();
    run:
        try {
            SocketController server;
            server.socketRun();
        }
        catch (std::exception e){
            goto run;
        }
}
