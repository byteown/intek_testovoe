#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <thread>

#include "agent/platform_monitor.h"

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    std::unique_ptr<IPlatformMonitor> monitor = createPlatformMonitor();

    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now();

    while (true) {
        FocusInfo info = monitor->foregroundWindow();

        bool user_active = false;

        std::cout << "Process name: " << info.process_name << std::endl;
        std::cout << "Window title: " << info.window_title << std::endl;

        if (monitor->idleMillis() < 5000) {
            user_active = true;
        }

        std::cout << "User active: " << user_active << std::endl;

        next += std::chrono::seconds(5);
        std::this_thread::sleep_until(next);
    }

    return 0;
}
