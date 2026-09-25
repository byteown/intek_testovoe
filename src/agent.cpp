//
// Created by user on 22.09.2026.
//

#include "agent/agent.h"

#include <iostream>

#include "agent/config.h"
#include "agent/platform_monitor.h"

Agent::Agent() : monitor_(createPlatformMonitor()) {}

Agent::~Agent() {
    this->stop();
}

void Agent::start() {
    collector_ = std::thread(&Agent::collectorLoop, this);
    sender_ = std::thread(&Agent::senderLoop, this);
}

void Agent::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopped_ = true;
    }

    cv_.notify_all();
    if (collector_.joinable()) {
        collector_.join();
    }

    if (sender_.joinable()) {
        sender_.join();
    }
}

void Agent::collectorLoop() {
    std::chrono::steady_clock::time_point next = std::chrono::steady_clock::now();

    while (true) {
        FocusInfo info = monitor_->foregroundWindow();
        ActivitySample sample{getTime(), info.process_name, info.window_title, monitor_->idleMillis() < 5000};
        next += collect_interval;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            samples_.push_back(sample);
        }
        cv_.notify_all();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (cv_.wait_until(lock, next, [this]() {return stopped_;})) return;
        }
    }
}

void Agent::senderLoop() {
    std::vector<ActivitySample> activities;
    bool stopping;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, send_interval, [this](){return samples_.size() >= collect_limit || stopped_;});
            activities = std::vector(samples_.begin(), samples_.end());
            samples_.clear();
            stopping = stopped_;
        }

        if (!activities.empty()) {
            std::cout << buildBatch(monitor_->hostname(), activities) << std::endl;
        }

        if (stopping) return;
    }
}
