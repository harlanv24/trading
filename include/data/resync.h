#pragma once

#include "core/ids.h"
#include "core/time.h"
#include <optional>
#include <string>

struct Resync {
    SessionId session_id;
    MarketId market_id;
    Timestamp recv_ts;
    Timestamp capture_ns;
    std::optional<Timestamp> exchange_ts;
    std::optional<std::string> reason;
};
