#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <thread>

#include "agent/activity_sample.h"
#include "agent/platform_monitor.h"

int main() {
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    std::unique_ptr<IPlatformMonitor> monitor = createPlatformMonitor();

    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now();

    std::vector<ActivitySample> activities;

    FocusInfo info = monitor->foregroundWindow();
    activities.push_back(ActivitySample{getTime(), info.process_name, info.window_title, monitor->idleMillis() < 5000});
    next += std::chrono::seconds(5);
    std::this_thread::sleep_until(next);

    info = monitor->foregroundWindow();
    activities.push_back(ActivitySample{getTime(), info.process_name, info.window_title, monitor->idleMillis() < 5000});
    next += std::chrono::seconds(5);
    std::this_thread::sleep_until(next);

    info = monitor->foregroundWindow();
    activities.push_back(ActivitySample{getTime(), info.process_name, info.window_title, monitor->idleMillis() < 5000});
    next += std::chrono::seconds(5);
    std::this_thread::sleep_until(next);

    std::cout << buildBatch(monitor->hostname(), activities) << std::endl;

    return 0;
}
