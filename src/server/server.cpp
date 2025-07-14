// server.cpp
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "expression.h"

constexpr int MAX_EVENTS = 1024;
constexpr int BUFFER_SIZE = 4096;

void setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./server <port>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);

    int serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD == -1)
    {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverFD, (sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(serverFD, SOMAXCONN) < 0)
    {
        perror("listen");
        return 1;
    }

    int epollFD = epoll_create1(0);
    epoll_event ev{}, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = serverFD;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, serverFD, &ev);

    std::unordered_map<int, std::string> buffers;

    std::cout << "Server listening on port " << port << "...\n";

    while (true)
    {
        int n = epoll_wait(epollFD, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;

            if (fd == serverFD)
            {
                int client_fd = accept(serverFD, nullptr, nullptr);
                if (client_fd < 0)
                    continue;
                setNonBlocking(client_fd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                epoll_ctl(epollFD, EPOLL_CTL_ADD, client_fd, &ev);
                buffers[client_fd] = "";
            }
            else
            {
                char buf[BUFFER_SIZE];
                int bytes = read(fd, buf, BUFFER_SIZE);
                if (bytes <= 0)
                {
                    close(fd);
                    buffers.erase(fd);
                    continue;
                }
                
                buffers[fd] += std::string(buf, bytes);
                // Check for complete expressions (split by space)
                size_t pos;
                while ((pos = buffers[fd].find(' ')) != std::string::npos)
                {
                    std::string expr = buffers[fd].substr(0, pos);
                    buffers[fd].erase(0, pos + 1);
                    try
                    {
                        Expression e(expr);
                        float result = e.evaluate();
                        std::string reply = std::to_string(result) + "\n";
                        send(fd, reply.c_str(), reply.size(), 0);
                    }
                    catch (const std::exception &ex)
                    {
                        std::string err = std::string("error: ") + ex.what() + "\n";
                        send(fd, err.c_str(), err.size(), 0);
                    }
                }
            }
        }
    }

    return 0;
}
