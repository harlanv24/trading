#pragma once
#include "core/ids.h"
#include "core/price.h"
#include "core/side.h"
#include "core/size.h"
#include "core/time.h"
#include <optional>

struct Delta {
    SessionId session_id;
    MarketId market_id;
    Timestamp recv_ts;
    Timestamp capture_ns;
    std::optional<Timestamp> exchange_ts;
    std::optional<bool> neg_risk;
    Side side;
    Price price;
    Size size;
};