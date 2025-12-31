#pragma once

#include "core/ids.h"
#include "core/price.h"
#include "core/side.h"
#include "core/size.h"
#include "core/time.h"
#include <optional>

struct Trade {
    SessionId session_id;
    MarketId market_id;
    std::optional<Timestamp> exchange_ts;
    Timestamp recv_ts;
    Timestamp capture_ns;
    Side side;
    Price price;
    Size size;
};