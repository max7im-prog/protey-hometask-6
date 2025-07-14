// client.cpp
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <future>
#include <unordered_map>
#include "expression.h"

std::string generateExpression(int n, std::mt19937 &rng)
{
    std::uniform_real_distribution<float> numDist(-100, 100);
    std::uniform_int_distribution<int> opDist(0, 3);
    std::string ops = "+-*/";
    std::string expr = "(" + std::to_string(numDist(rng)) + ")";
    for (int i = 1; i < n; ++i)
    {
        char op = ops[opDist(rng)];
        expr = expr + op + "(" + std::to_string(numDist(rng)) + ")";
    }
    return expr;
}

std::vector<std::string> fragmentExpression(const std::string &expr, std::mt19937 &rng)
{
    std::uniform_int_distribution<int> fragDist(1, expr.size() / 2 + 1);
    std::vector<std::string> fragments;
    size_t i = 0;
    while (i < expr.size())
    {
        int len = std::min<size_t>(fragDist(rng), expr.size() - i);
        fragments.push_back(expr.substr(i, len));
        i += len;
    }
    fragments.back() += " "; // add space as expression terminator
    return fragments;
}

int connectToServer(const std::string &addr, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, addr.c_str(), &server.sin_addr);
    connect(sock, (sockaddr *)&server, sizeof(server));
    return sock;
}

void setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void runClient(int n, int connections, const std::string &addr, int port)
{
    std::mt19937 rng(std::random_device{}());
    int epollFD = epoll_create1(0);
    if (epollFD < 0)
    {
        perror("epoll_create1");
        return;
    }

    struct ConnState
    {
        std::string expr;
        float expected;
        std::string received;
    };

    std::unordered_map<int, ConnState> connStates;

    for (int i = 0; i < connections; ++i)
    {
        std::string expr = generateExpression(n, rng);
        std::vector<std::string> fragments = fragmentExpression(expr, rng);
        Expression verifier(expr);
        float expected = verifier.evaluate();

        int sock = connectToServer(addr, port);
        if (sock < 0)
        {
            std::cerr << "Connection failed\n";
            continue;
        }

        setNonBlocking(sock);

        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.fd = sock;
        if (epoll_ctl(epollFD, EPOLL_CTL_ADD, sock, &ev) < 0)
        {
            perror("epoll_ctl");
            close(sock);
            continue;
        }

        // Send expression fragments
        for (const std::string &frag : fragments)
        {
            send(sock, frag.c_str(), frag.size(), 0);
        }

        connStates[sock] = ConnState{expr, expected, ""};
    }

    const int MAX_EVENTS = 32;
    epoll_event events[MAX_EVENTS];
    int responsesReceived = 0;

    while (responsesReceived < connStates.size())
    {
        int n = epoll_wait(epollFD, events, MAX_EVENTS, -1);
        if (n < 0)
        {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < n; ++i)
        {
            int fd = events[i].data.fd;
            char buf[1024];
            int bytes = recv(fd, buf, sizeof(buf) - 1, 0);

            if (bytes <= 0)
            {
                close(fd);
                continue;
            }

            buf[bytes] = '\0';
            connStates[fd].received += std::string(buf);

            // Assume one full response per socket
            try
            {
                float result = std::stof(connStates[fd].received);
                std::cout << "Expression: " << connStates[fd].expr << "\n";
                std::cout << "Expected:   " << connStates[fd].expected << "\n";
                std::cout << "Received:   " << result << "\n";

                if (std::abs(result - connStates[fd].expected) > 1e-3)
                {
                    std::cerr << "ERROR: Mismatch!\n";
                }

                responsesReceived++;
                close(fd);
            }
            catch (...)
            {
                // Not yet a valid float? Wait for more data
            }
        }
    }

    close(epollFD);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./client <n> <connections> <server_addr> <server_port>\n";
        return 1;
    }

    int n = std::stoi(argv[1]);
    int connections = std::stoi(argv[2]);
    std::string addr = argv[3];
    int port = std::stoi(argv[4]);

    runClient(n, connections, addr, port);
    return 0;
}
