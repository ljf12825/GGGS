#include "network/TcpServer.h"
#include <iostream>

int main() {
    TcpServer server;

    if (!server.Init(9000)) {
        std::cout << "Server init failed\n";
        return -1;
    }

    std::cout << "GameServer start...\n";

    server.Run();

    return 0;
}