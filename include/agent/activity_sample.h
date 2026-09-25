//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_ACTIVITY_SAMPLE_H
#define INTEK_TESTOVOE_ACTIVITY_SAMPLE_H
#include <string>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include "nlohmann/json.hpp"


struct ActivitySample {
    std::string time;
    std::string process_name;
    std::string window_title;
    bool user_active;
};

inline void to_json(nlohmann::ordered_json& j, const ActivitySample& s) {
    j["time"] = s.time;
    j["process_name"] = s.process_name;
    j["window_title"] = s.window_title;
    j["user_active"] = s.user_active;
}

inline void from_json(const nlohmann::ordered_json& j, ActivitySample& s) {
    s.time = j.at("time").get<std::string>();
    s.process_name = j.at("process_name").get<std::string>();
    s.window_title = j.at("window_title").get<std::string>();
    s.user_active = j.at("user_active").get<bool>();
}

inline std::string getTime() {
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();

    std::time_t t = std::chrono::system_clock::to_time_t(tp);

    std::tm tm = *std::localtime(&t);

    std::ostringstream result;
    result << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return result.str();
}

inline std::string buildBatch(const std::string& agent_id, const std::vector<ActivitySample>& payload) {
    nlohmann::ordered_json j;

    j["agent_id"] = agent_id;
    j["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    j["payload"] = payload;

    return j.dump();
}


#endif //INTEK_TESTOVOE_ACTIVITY_SAMPLE_H
