//
// Created by user on 22.09.2026.
//

#ifndef INTEK_TESTOVOE_DISK_BUFFER_H
#define INTEK_TESTOVOE_DISK_BUFFER_H
#include <vector>

#include "activity_sample.h"


class DiskBuffer {
public:
    std::vector<ActivitySample> load();
    int save(const std::vector<ActivitySample> &activities);
    void clear();
};


#endif //INTEK_TESTOVOE_DISK_BUFFER_H
