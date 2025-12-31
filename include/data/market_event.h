#pragma once

#include "data/event_type.h"
#include "data/heartbeat.h"
#include "data/resync.h"
#include "data/trade.h"
#include "delta.h"
#include "record_meta.h"
#include "snapshot.h"
#include <variant>

struct MarketEvent {
    EventType type;
    std::variant<RecordMeta, Snapshot, Delta, Trade, Resync, Heartbeat> payload;
};
