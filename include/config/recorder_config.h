#pragma once
#include <string>

struct RecorderConfig {
    std::string rest_url;
    std::string ws_url;
    int64_t snapshot_interval_seconds;
};