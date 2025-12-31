#pragma once

#include "config/recorder_config.h"
#include "core/ids.h"
#include "core/time.h"
#include <optional>

struct RecordMeta {
    SessionId session_id;
    MarketId market_id;
    RecorderVersion recorder_version;
    Timestamp start_ts;
    std::optional<Timestamp> exchange_ts;
    std::optional<bool> neg_risk;
    RecorderConfig config;
};
