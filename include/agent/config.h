//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_CONFIG_H
#define INTEK_TESTOVOE_CONFIG_H
#include <chrono>


constexpr auto collect_interval = std::chrono::seconds(5);
constexpr auto send_interval = std::chrono::seconds(30);
constexpr std::size_t collect_limit = 10;


#endif //INTEK_TESTOVOE_CONFIG_H
