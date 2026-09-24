//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_PLATFORM_MONITOR_H
#define INTEK_TESTOVOE_PLATFORM_MONITOR_H
#include <cstdint>
#include <memory>
#include <string>

struct FocusInfo { std::string process_name; std::string window_title; };

class IPlatformMonitor {
public:
    virtual ~IPlatformMonitor() = default;
    virtual FocusInfo       foregroundWindow() = 0;
    virtual uint64_t        idleMillis()       = 0;
    virtual std::string     hostname()         = 0;
};

std::unique_ptr<IPlatformMonitor> createPlatformMonitor();

#endif //INTEK_TESTOVOE_PLATFORM_MONITOR_H
