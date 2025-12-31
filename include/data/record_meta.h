#pragma once

#include "config/recorder_config.h"
#include "core/ids.h"
#include "core/time.h"

struct RecordMeta {
    SessionId session_id;
    MarketId market_id;
    RecorderVersion recorder_version;
    Timestamp start_ts;
    RecorderConfig config;
};
