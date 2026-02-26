#include <unordered_map>
#include <memory>
#include "../core/Session.h"

class TcpServer {
private:
    int listen_fd{-1};

    std::unordered_map<int, std::unique_ptr<Session>> sessions;

    void AcceptClient();
    void ReceiveClient(int fd);

public:
    virtual ~TcpServer();

    bool Init(int port);

    void Run();
};