//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_AGENT_H
#define INTEK_TESTOVOE_AGENT_H
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

#include "activity_sample.h"
#include "platform_monitor.h"


class Agent {
public:
    Agent();
    ~Agent();

    void start();
    void stop();
private:
    std::unique_ptr<IPlatformMonitor> monitor_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::deque<ActivitySample> samples_;
    bool stopped_ = false;
    std::thread collector_, sender_;

    void collectorLoop();
    void senderLoop();
};


#endif //INTEK_TESTOVOE_AGENT_H
