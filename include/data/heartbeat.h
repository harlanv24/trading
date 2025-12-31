#pragma once

#include "core/ids.h"
#include "core/time.h"

struct Heartbeat {
    SessionId session_id;
    MarketId market_id;
    Timestamp recv_ts;
    Timestamp capture_ns;
};