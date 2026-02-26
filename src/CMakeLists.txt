file(GLOB_RECURSE SOURCES
    core/*.h
    network/*.cpp
    main.cpp
)

add_executable(GGGS
    main.cpp
    network/TcpServer.cpp
)

target_link_libraries(GGGS pthread)