```text
202602261900
项目需要epoll，但客户端是Unity写的，需要在Windows上的Linux环境进行开发，WSL成为首选
WSL + VSCode开发环境配置
```

```text
202602262000
第一版

src/
├── core/
│   ├── Session.h
│   ├── MessageDispatcher.h
│   ├── GameServer.h
│
├── network/
│   ├── TcpServer.h
│   ├── TcpServer.cpp
│
├── main.cpp
```