#ifdef _WIN32
#include <windows.h>
#endif

#include <csignal>
#include <atomic>

#include "agent/agent.h"

std::atomic<bool> stopping{false};

void setStop(int v) {
    stopping = true;
}

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    std::signal(SIGINT, setStop);
    std::signal(SIGTERM, setStop);

    Agent agent;
    agent.start();

    while (!stopping) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    agent.stop();

    return 0;
}
