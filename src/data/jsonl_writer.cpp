#define RAPIDJSON_HAS_STDSTRING 1

#include "data/jsonl_writer.h"
#include "book/order_book.h"
#include "core/ids.h"
#include "data/delta.h"
#include "data/event_type.h"
#include "data/heartbeat.h"
#include "data/market_event.h"
#include "data/record_meta.h"
#include "data/resync.h"
#include "data/snapshot.h"
#include "data/trade.h"
#include <ios>
#include <optional>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <stdexcept>

namespace {
using RapidWriter = rapidjson::Writer<rapidjson::StringBuffer>;
using rapidjson::StringRef;

// Common constants across multiple EventTypes (except RecordMeta)
const char *kEventType = "event_type";
const char *kSessionId = "session_id";
const char *kMarketId = "market_id";
const char *kRecvTs = "recv_ts";
const char *kCaptureNs = "capture_ns";
const char *kExchangeTs = "exchange_ts";
}; // namespace

JsonlWriter::JsonlWriter(const std::string &path) : out_(path, std::ios::app | std::ios::binary) {
    if (!out_) {
        throw std::runtime_error("Failed to open JSONL file: " + path);
    }
}

bool JsonlWriter::good() const {
    return out_.good();
}

void JsonlWriter::flush() {
    out_.flush();
}

JsonlWriter::~JsonlWriter() {
    flush();
}

const char *EventTypeToString(const EventType &t) {
    switch (t) {
    case EventType::RecordMeta:
        return "record_meta";
    case EventType::Snapshot:
        return "snapshot";
    case EventType::Delta:
        return "delta";
    case EventType::Trade:
        return "trade";
    case EventType::Resync:
        return "resync";
    case EventType::Heartbeat:
        return "heartbeat";
    }
    return "unknown";
}

const char *SideToString(const Side &s) {
    switch (s) {
    case Side::Ask:
        return "ask";
    case Side::Bid:
        return "bid";
    }
    return "unknown";
}

void WriteCommonFields(RapidWriter &w, const SessionId &sid, const MarketId &mid,
                       std::optional<Timestamp> exchange_ts, Timestamp recv_ts,
                       Timestamp capture_ns) {
    w.Key(StringRef(kSessionId));
    w.String(StringRef(sid.value));
    w.Key(StringRef(kMarketId));
    w.String(StringRef(mid.value));
    if (exchange_ts) {
        w.Key(StringRef(kExchangeTs));
        w.Int64(exchange_ts->ns().count());
    }
    w.Key(StringRef(kRecvTs));
    w.Int64(recv_ts.ns().count());
    w.Key(StringRef(kCaptureNs));
    w.Int64(capture_ns.ns().count());
}

void JsonlWriter::write(const MarketEvent &e) {
    rapidjson::StringBuffer buffer;
    RapidWriter writer = RapidWriter(buffer);
    writer.StartObject();
    writer.Key(StringRef(kEventType));
    writer.String(EventTypeToString(e.type));

    switch (e.type) {
    case EventType::RecordMeta: {
        const auto &p = std::get<RecordMeta>(e.payload);
        writer.Key(StringRef(kSessionId));
        writer.String(p.session_id.value);
        writer.Key(StringRef(kMarketId));
        writer.String(p.market_id.value);
        writer.Key("recorder_version");
        writer.String(p.recorder_version.value);
        writer.Key("start_ts");
        writer.Int64(p.start_ts.ns().count());
        writer.Key("config");
        writer.StartObject();
        writer.Key("rest_url");
        writer.String(p.config.rest_url);
        writer.Key("ws_url");
        writer.String(p.config.ws_url);
        writer.Key("snapshot_interval_seconds");
        writer.Int64(p.config.snapshot_interval_seconds);
        writer.EndObject();
        break;
    }
    case EventType::Snapshot: {
        const auto &p = std::get<Snapshot>(e.payload);
        WriteCommonFields(writer, p.session_id, p.market_id, p.exchange_ts, p.recv_ts,
                          p.capture_ns);
        if (p.neg_risk) {
            writer.Key("neg_risk");
            writer.Bool(*p.neg_risk);
        }
        writer.Key("bids");
        writer.StartArray();
        for (const auto &level : p.book_state.bids) {
            if (level.second.ticks() <= 0) {
                throw std::runtime_error("Encountered a Bid of size 0 for price " +
                                         std::to_string(level.first.ticks()));
            }
            writer.StartArray();
            writer.Int64(level.first.ticks());
            writer.Int64(level.second.ticks());
            writer.EndArray();
        }
        writer.EndArray();
        writer.Key("asks");
        writer.StartArray();
        for (const auto &level : p.book_state.asks) {
            if (level.second.ticks() <= 0) {
                throw std::runtime_error("Encountered an Ask of size 0 for price " +
                                         std::to_string(level.first.ticks()));
            }
            writer.StartArray();
            writer.Int64(level.first.ticks());
            writer.Int64(level.second.ticks());
            writer.EndArray();
        }
        writer.EndArray();
        break;
    }
    case EventType::Delta: {
        const auto &p = std::get<Delta>(e.payload);
        WriteCommonFields(writer, p.session_id, p.market_id, p.exchange_ts, p.recv_ts,
                          p.capture_ns);
        writer.Key("side");
        writer.String(SideToString(p.side));
        writer.Key("price_ticks");
        writer.Int64(p.price.ticks());
        writer.Key("size_ticks");
        writer.Int64(p.size.ticks());
        break;
    }
    case EventType::Trade: {
        const auto &p = std::get<Trade>(e.payload);
        WriteCommonFields(writer, p.session_id, p.market_id, p.exchange_ts, p.recv_ts,
                          p.capture_ns);
        writer.Key("side");
        writer.String(SideToString(p.side));
        writer.Key("price_ticks");
        writer.Int64(p.price.ticks());
        writer.Key("size_ticks");
        writer.Int64(p.size.ticks());
        break;
    }
    case EventType::Resync: {
        const auto &p = std::get<Resync>(e.payload);
        WriteCommonFields(writer, p.session_id, p.market_id, p.exchange_ts, p.recv_ts,
                          p.capture_ns);
        if (p.reason) {
            writer.Key("reason");
            writer.String(StringRef(*p.reason));
        }
        break;
    }
    case EventType::Heartbeat: {
        const auto &p = std::get<Heartbeat>(e.payload);
        WriteCommonFields(writer, p.session_id, p.market_id, p.exchange_ts, p.recv_ts,
                          p.capture_ns);
        if (p.note) {
            writer.Key("note");
            writer.String(StringRef(*p.note));
        }

        break;
    }
    }
    writer.EndObject();
    out_.write(buffer.GetString(), buffer.GetSize());
    out_.put('\n');
}
