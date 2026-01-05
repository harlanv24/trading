#define RAPIDJSON_HAS_STDSTRING 1

#include "data/jsonl_reader.h"
#include "core/ids.h"
#include "data/delta.h"
#include "data/event_type.h"
#include "data/heartbeat.h"
#include "data/market_event.h"
#include "data/record_meta.h"
#include "data/resync.h"
#include "data/snapshot.h"
#include "data/trade.h"
#include "rapidjson/rapidjson.h"
#include <optional>
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {
using RapidReader = rapidjson::Reader;

// Common constants across multiple EventTypes (except RecordMeta)
const char *kEventType = "event_type";
const char *kSessionId = "session_id";
const char *kMarketId = "market_id";
const char *kRecvTs = "recv_ts";
const char *kCaptureNs = "capture_ns";
const char *kExchangeTs = "exchange_ts";

std::optional<EventType> StringToEventType(std::string_view s) {
    if (s == "record_meta")
        return EventType::RecordMeta;
    if (s == "snapshot")
        return EventType::Snapshot;
    if (s == "delta")
        return EventType::Delta;
    if (s == "trade")
        return EventType::Trade;
    if (s == "resync")
        return EventType::Resync;
    if (s == "heartbeat")
        return EventType::Heartbeat;
    return std::nullopt;
}

struct MarketEventHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MarketEventHandler> {
    MarketEvent output;

    bool Key(const char *str, rapidjson::SizeType len, bool) {
        current_key_.assign(str, len);
        if (current_key_ != "event_type" and !current_event_type_) {
            return false;
        }
        return true;
    }

    bool String(const char *str, rapidjson::SizeType len, bool) {
        std::string_view v(str, len);
        if (current_key_ == "event_type") {
            auto et = StringToEventType(v);
            if (!et)
                return false; // unknown event
            current_event_type_ = *et;
        }
        return true;
    }

  private:
    std::string current_key_;
    std::optional<EventType> current_event_type_;
};
}; // namespace

JsonlReader::JsonlReader(const std::string &path)
    : in_(path, std::ios::in | std::ios::binary), line_number_(0) {
    if (!in_) {
        throw std::runtime_error("Failed to open JSONL file: " + path);
    }
}

std::optional<MarketEvent> JsonlReader::next_event() {
    std::string line;
    line_number_++;
    if (!std::getline(in_, line)) {
        if (in_.eof()) {
            return std::nullopt;
        } else {
            throw std::runtime_error("Called next_event but could not find input on line " +
                                     std::to_string(line_number_));
        }
    }
    RapidReader reader;
    rapidjson::StringStream ss(line.c_str());
    MarketEventHandler handler;
    if (!reader.Parse(ss, handler)) {
        throw std::runtime_error("Line " + std::to_string(line_number_) + "parse error " +
                                 std::to_string(reader.GetParseErrorCode()) + " at offset " +
                                 std::to_string(reader.GetErrorOffset()));
    }
    return handler.output;
}

JsonlReader::~JsonlReader() = default;
