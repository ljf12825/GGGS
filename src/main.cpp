#include "network/TcpServer.h"
#include <iostream>

class GameSession : public Session {
public:
    GameSession(int fd, MessageDispatcher* disp) : Session(fd, disp) {}
    
    void OnReceive(const std::string& msg) override {
        std::cout << "Session " << fd << " received: " << msg << std::endl;
        Session::OnReceive(msg);
    }
};

void onLogin(Session* session, const std::string& payload) {
    std::cout << "Login request: " << payload << std::endl;
    session->Send("Login success");
}

void onMove(Session* session, const std::string& payload) {
    std::cout << "Move command: " << payload << std::endl;
    session->Send("Move executed");
}

int main() {
    TcpServer server;

    server.RegisterMessage(1, onLogin);
    server.RegisterMessage(2, onMove);

    if (!server.Init(9000)) {
        std::cout << "Server init failed\n";
        return -1;
    }

    std::cout << "GameServer start on port 9000...\n";

    server.Run();

    return 0;
}