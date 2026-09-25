//
// Created by user on 22.09.2026.
//

#include "agent/disk_buffer.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include "nlohmann/json.hpp"

#include "agent/config.h"

std::vector<ActivitySample> DiskBuffer::load() {
    std::ifstream in(file_buffer_path);

    if (!in) return {};

    try {
        auto res = nlohmann::ordered_json::parse(in).get<std::vector<ActivitySample>>();
        return res;
    } catch (const::std::exception& e) {
        std::cerr << e.what() << std::endl;
        return {};
    }
}

int DiskBuffer::save(const std::vector<ActivitySample> &activities) {
    std::vector<ActivitySample> res;
    int n;

    if (activities.size() > buffer_limit) {
        res = std::vector<ActivitySample>(activities.end() - buffer_limit, activities.end());
        n = res.size();
    } else {
        res = std::vector<ActivitySample>(activities.begin(), activities.end());
        n = res.size();
    }

    nlohmann::ordered_json j = res;

    std::string path = file_buffer_path;
    path.append(".tmp");

    std::ofstream out(path);
    out << j << std::endl;

    std::error_code ec;

    if (!out) {
        std::cerr << "Unable to open file" << std::endl;
        std::filesystem::remove(path, ec);
        return 0;
    }

    out.close();

    std::filesystem::rename(path, file_buffer_path, ec);

    if (ec) {
        std::cerr << ec.message() << std::endl;
    }

    return n;
}

void DiskBuffer::clear() {
    std::error_code ec;
    std::filesystem::remove(file_buffer_path, ec);
}
