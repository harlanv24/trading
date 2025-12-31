#pragma once

#include "book/order_book.h"
#include "core/ids.h"
#include "core/time.h"
#include <optional>

struct Snapshot {
    SessionId session_id;
    MarketId market_id;
    Timestamp recv_ts;
    Timestamp capture_ns;
    std::optional<Timestamp> exchange_ts;
    std::optional<bool> neg_risk;
    OrderBook book_state;
};
